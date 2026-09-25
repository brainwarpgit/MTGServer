
#ifndef SCHEDULESHUTTLETASK_H_
#define SCHEDULESHUTTLETASK_H_

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/planet/PlanetManager.h"
#include "server/zone/TravelStartupTask.h"
#include "server/zone/Zone.h"

#include <exception>

class ScheduleShuttleTask : public Task, public Logger {
	ManagedWeakReference<CreatureObject*> shuttleObject;
	ManagedWeakReference<Zone*> zoneReference;
	Reference<TravelStartupTask*> startup;
	uint64 shuttleID;

	struct StartupRegistration {
		TravelStartupTask* task;
		uint64 id;
		bool pending = false;
		bool failed = true;
		bool discarded = false;
		String reason = "Registration exited unexpectedly.";

		~StartupRegistration() {
			if (task != nullptr && !pending)
				task->finishRegistration(id, failed, discarded, reason);
		}
	};

public:
	ScheduleShuttleTask(CreatureObject* shuttle, Zone* zon, TravelStartupTask* startupTask = nullptr) : Task() {
		shuttleObject = shuttle;
		zoneReference = zon;
		startup = startupTask;
		shuttleID = shuttle->getObjectID();

		Logger::setLoggingName("ScheduleShuttleTask");
	}

	void run() {
		// Complete failed registrations on every early return or exception. The
		// loading retry is the only path that deliberately keeps one pending.
		StartupRegistration registration{startup.get(), shuttleID};

		try {
			runRegistration(registration);
		} catch (const Exception& exception) {
			registration.reason = "Exception during registration: " + exception.getMessage();
			throw;
		} catch (const std::exception& exception) {
			registration.reason = String("Exception during registration: ") + exception.what();
			throw;
		} catch (...) {
			registration.reason = "Unknown exception during registration.";
			throw;
		}
	}

private:
	void runRegistration(StartupRegistration& registration) {
		ManagedReference<Zone*> zone = zoneReference.get();

		if (zone == nullptr) {
			registration.reason = "Zone expired before shuttle registration.";
			error() << " zone has a nullptr.";
			return;
		}

		auto zoneServer = zone->getZoneServer();

		if (zoneServer == nullptr) {
			registration.reason = "Zone server unavailable during shuttle registration.";
			error() << " zoneServer is nullptr.";
			return;
		}

		if (zoneServer->isServerLoading()) {
			schedule(1000);
			registration.pending = true;
			return;
		}

		if (zoneServer->isServerOffline() || zoneServer->isServerShuttingDown()) {
			registration.reason = "Server went offline or began shutdown before shuttle registration.";
			return;
		}

		ManagedReference<CreatureObject*> strongShuttle = shuttleObject.get();

		if (strongShuttle == nullptr) {
			registration.reason = "Shuttle expired before registration.";
			error() << " Shuttle strongShuttle has a nullptr in Zone: " << zone->getZoneName();
			return;
		}

		Locker lock(strongShuttle);

		if (strongShuttle->getZone() != zone.get()) {
			registration.reason = "Shuttle left its startup zone before registration.";
			return;
		}

		ManagedReference<PlanetManager*> planetManager = zone->getPlanetManager();

		if (planetManager == nullptr) {
			registration.reason = "Planet manager unavailable during shuttle registration.";
			zone->error() << " planetManager has a nullptr in Zone: " << zone->getZoneName();
			return;
		}

		ManagedReference<CityRegion*> cityRegion = strongShuttle->getCityRegion().get();

		// Player City
		if ((cityRegion != nullptr) && !cityRegion->isClientRegion()) {
			float x = strongShuttle->getWorldPositionX();
			float y = strongShuttle->getWorldPositionY();
			float z = strongShuttle->getWorldPositionZ();

			Vector3 arrivalVector(x, y, z);

			String zoneName = zone->getZoneName();

			Locker clocker(cityRegion, strongShuttle);

			cityRegion->setShuttleID(strongShuttle->getObjectID());
			clocker.release();

			PlanetTravelPoint* planetTravelPoint = new PlanetTravelPoint(zoneName, cityRegion->getCityRegionName(), arrivalVector, arrivalVector, strongShuttle, 6.f);

			planetManager->addPlayerCityTravelPoint(planetTravelPoint);

			if (startup != nullptr)
				startup->setTravelPoint(shuttleID, planetTravelPoint->getPointName(), true);

			planetManager->scheduleShuttle(strongShuttle, PlanetManager::SHUTTLEPORT);
		} else {
			Reference<PlanetTravelPoint*> travelPoint = planetManager->getNearestPlanetTravelPoint(strongShuttle, 128.f);

			if (travelPoint == nullptr) {
				registration.reason = "No configured travel point within 128 meters.";
				error() << " Planet Travel Point (travelPoint) has a nullptr in Zone: " << zone->getZoneName();
				return;
			}

			if (startup != nullptr)
				startup->setTravelPoint(shuttleID, travelPoint->getPointName(), false);

			auto oldShuttle = travelPoint->getShuttle();

			if (oldShuttle == nullptr) {
				travelPoint->setShuttle(strongShuttle);

				if (startup != nullptr)
					startup->setTravelPoint(shuttleID, travelPoint->getPointName(), true);

				if (travelPoint->isInterplanetary()) {
					planetManager->scheduleShuttle(strongShuttle, PlanetManager::STARPORT);
				} else {
					planetManager->scheduleShuttle(strongShuttle, PlanetManager::SHUTTLEPORT);
				}
			} else if (oldShuttle != strongShuttle) {
				strongShuttle->destroyObjectFromWorld(true);
				strongShuttle->destroyObjectFromDatabase(true);
				registration.discarded = true;
				registration.reason = "Duplicate shuttle discarded; travel point already uses shuttle 0x" + String::hexvalueOf(oldShuttle->getObjectID()) + ".";
			} else if (startup != nullptr) {
				startup->setTravelPoint(shuttleID, travelPoint->getPointName(), true);
			}
		}

		registration.failed = false;
	}
};

#endif /* SCHEDULESHUTTLETASK_H_ */
