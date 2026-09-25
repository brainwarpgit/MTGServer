/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions. */

#include "MustafarMapBoundary.h"

#include "conf/ConfigManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/planet/PlanetManager.h"
#include "server/zone/managers/planet/PlanetTravelPoint.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/ValidatedPosition.h"
#include "server/zone/objects/scene/SceneObject.h"

using namespace server::zone;
using namespace server::zone::objects::creature;
using namespace server::zone::objects::scene;

namespace {
	bool applies(Zone* zone) {
		return zone != nullptr && zone->getZoneName() == "mustafar" && ConfigManager::instance()->getBool("Core3.MustafarMapBounds", true);
	}

	bool isInside(Zone* zone, const Vector3& position, uint64 parentID) {
		if (!std::isfinite(position.getZ())) {
			return false;
		}

		if (parentID == 0) {
			return MustafarMapBoundary::contains(position.getX(), position.getY());
		}

		auto zoneServer = zone->getZoneServer();
		if (zoneServer == nullptr) {
			return false;
		}

		ManagedReference<SceneObject*> parent = zoneServer->getObject(parentID);
		if (parent == nullptr || !parent->isCellObject() || parent->getZone() != zone || parent->getRootParent() == nullptr) {
			return false;
		}

		ValidatedPosition local(position);
		local.setParent(parentID);
		const auto world = local.getWorldPosition(zoneServer);
		return MustafarMapBoundary::contains(world.getX(), world.getY());
	}

	void stopMovement(CreatureObject* player) {
		player->setCurrentSpeed(0.f);
		player->updateLocomotion();

		ManagedReference<SceneObject*> parent = player->getParent().get();
		if (parent != nullptr && (parent->isVehicleObject() || parent->isMount())) {
			Locker locker(parent, player);
			auto mount = parent->asCreatureObject();
			if (mount != nullptr) {
				mount->setCurrentSpeed(0.f);
				mount->updateLocomotion();
			}
		}
	}
}

bool MustafarMapBoundary::resolveDestination(CreatureObject* player, Zone* zone, Vector3& position, uint64& parentID) {
	if (player == nullptr || !applies(zone) || isInside(zone, position, parentID)) {
		return true;
	}

	stopMovement(player);
	bool recovered = false;

	// Only reuse positions while the player is currently inside this same zone.
	// A cross-planet transfer or a saved outside position must use the live port.
	if (player->getZone() == zone) {
		ValidatedPosition current;
		current.update(player);
		if (isInside(zone, current.getPosition(), current.getParent())) {
			auto ghost = player->getPlayerObject();
			auto previous = ghost != nullptr ? ghost->getLastValidatedPosition() : nullptr;
			if (previous != nullptr && isInside(zone, previous->getPosition(), previous->getParent())) {
				current = *previous;
			}
			position = current.getPosition();
			parentID = current.getParent();
			recovered = true;
		}
	}

	if (!recovered) {
		auto planetManager = zone->getPlanetManager();
		if (planetManager != nullptr) {
			Reference<PlanetTravelPoint*> point = planetManager->getPlanetTravelPoint("Mensix Mining Facility");
			if (point != nullptr && point->getPointZone() == "mustafar" && isInside(zone, point->getArrivalPosition(), 0)) {
				position = point->getArrivalPosition();
				parentID = 0;
				recovered = true;
			}
		}
	}

	if (!recovered) {
		player->sendSystemMessage("That location is outside Mustafar's mapped area. No safe return point is configured; contact an administrator.");
		return false;
	}

	player->sendSystemMessage("You have reached the edge of Mustafar's mapped area. Returning you to a safe location.");
	return true;
}

bool MustafarMapBoundary::enforceMovement(CreatureObject* player, const Vector3& position, uint64 parentID) {
	auto zone = player->getZone();
	if (!applies(zone) || isInside(zone, position, parentID)) {
		return true;
	}

	Vector3 recovery = position;
	if (resolveDestination(player, zone, recovery, parentID)) {
		// Use the normal teleport acknowledgement and mount synchronization path.
		player->teleport(recovery.getX(), recovery.getZ(), recovery.getY(), parentID);
	}
	return false;
}
