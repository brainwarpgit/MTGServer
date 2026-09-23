/*
 * VehicleControlDeviceImplementation.cpp
 *
 *  Created on: 10/04/2010
 *      Author: victor
 */

#include "server/zone/objects/intangible/VehicleControlDevice.h"
#include "server/zone/objects/intangible/VehicleControlObserver.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/VehicleObject.h"
#include "server/zone/objects/creature/JetpackTemplate.h"
#include "server/zone/objects/creature/events/VehicleDecayTask.h"
#include "server/zone/packets/scene/AttributeListMessage.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/Zone.h"
#include "tasks/CallMountTask.h"
#include "server/zone/objects/region/CityRegion.h"
#include "server/zone/objects/player/sessions/TradeSession.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/player/PlayerObject.h"

void VehicleControlDeviceImplementation::generateObject(CreatureObject* player) {
	if (player->isDead() || player->isIncapacitated())
		return;

	if (!isASubChildOf(player))
		return;

	if (player->getParent() != nullptr || player->isInCombat()) {
		player->sendSystemMessage("@pet/pet_menu:cant_call_vehicle"); // You can only unpack vehicles while Outside and not in Combat.
		return;
	}

	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject == nullptr || controlledObject->getLocalZone() != nullptr) {
		return;
	}

	auto ghost = player->getPlayerObject();

	if (ghost == nullptr) {
		return;
	}

	auto zoneServer = player->getZoneServer();

	if (zoneServer == nullptr) {
		return;
	}

	ManagedReference<TradeSession*> tradeContainer = player->getActiveSession(SessionFacadeType::TRADE).castTo<TradeSession*>();

	if (tradeContainer != nullptr) {
		auto playerManager = zoneServer->getPlayerManager();

		if (playerManager != nullptr) {
			playerManager->handleAbortTradeMessage(player);
		}
	}

	if (player->getPendingTask("call_mount") != nullptr) {
		StringIdChatParameter waitTime("pet/pet_menu", "call_delay_finish_vehicle");
		AtomicTime nextExecution;
		Core::getTaskManager()->getNextExecutionTime(player->getPendingTask("call_mount"), nextExecution);
		int timeLeft = (nextExecution.getMiliTime() / 1000) - System::getTime();
		waitTime.setDI(timeLeft);

		player->sendSystemMessage(waitTime);
		return;
	}

	ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

	if (datapad == nullptr) {
		return;
	}

	int currentlySpawned = 0;

	for (int i = 0; i < datapad->getContainerObjectsSize(); ++i) {
		ManagedReference<SceneObject*> object = datapad->getContainerObject(i);

		if (object->isVehicleControlDevice()) {
			VehicleControlDevice* device = cast<VehicleControlDevice*>(object.get());

			ManagedReference<SceneObject*> vehicle = device->getControlledObject();

			if (vehicle != nullptr && vehicle->isInQuadTree()) {
				if (++currentlySpawned > 2)
					player->sendSystemMessage("@pet/pet_menu:has_max_vehicle");

				return;
			}
		}
	}

	if (player->getCurrentCamp() == nullptr && player->getCityRegion() == nullptr && !ghost->isPrivileged()) {
		Reference<CallMountTask*> callMount = new CallMountTask(_this.getReferenceUnsafeStaticCast(), player, "call_mount");

		StringIdChatParameter message("pet/pet_menu", "call_vehicle_delay");
		message.setDI(15);
		player->sendSystemMessage(message);

		player->addPendingTask("call_mount", callMount, 15 * 1000);

		if (vehicleControlObserver == nullptr) {
			vehicleControlObserver = new VehicleControlObserver(_this.getReferenceUnsafeStaticCast());
			vehicleControlObserver->deploy();
		}

		player->registerObserver(ObserverEventType::STARTCOMBAT, vehicleControlObserver);

	} else {
		Locker clocker(controlledObject, player);
		spawnObject(player);
	}
}

void VehicleControlDeviceImplementation::spawnObject(CreatureObject* player) {
	ZoneServer* zoneServer = getZoneServer();

	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject == nullptr)
		return;

	if (!isASubChildOf(player))
		return;

	if (player->getParent() != nullptr || player->isInCombat()) {
		player->sendSystemMessage("@pet/pet_menu:cant_call_vehicle"); // You can only unpack vehicles while Outside and not in Combat.
		return;
	}

	auto zone = player->getZone();

	if (zone == nullptr || !zone->isGroundZone()) {
		return;
	}

	ManagedReference<TradeSession*> tradeContainer = player->getActiveSession(SessionFacadeType::TRADE).castTo<TradeSession*>();

	if (tradeContainer != nullptr) {
		server->getZoneServer()->getPlayerManager()->handleAbortTradeMessage(player);
	}

	Vector3 playerWorld = player->getWorldPosition();

	controlledObject->initializePosition(playerWorld.getX(), playerWorld.getZ(), playerWorld.getY());
	ManagedReference<CreatureObject*> vehicle = nullptr;

	if (controlledObject->isCreatureObject()) {
		vehicle = cast<CreatureObject*>(controlledObject.get());
		vehicle->setCreatureLink(player);
		vehicle->setControlDevice(_this.getReferenceUnsafeStaticCast());
	}

	zone->transferObject(controlledObject, -1, true);

	Reference<VehicleDecayTask*> decayTask = new VehicleDecayTask(controlledObject);

	if (decayTask != nullptr) {
		decayTask->execute();
	}

	if (vehicle != nullptr && JetpackTemplate::isJetpack(controlledObject->getServerObjectCRC())) {
		controlledObject->setCustomizationVariable("/private/index_hover_height", 40, true);				  // Illusion of flying.
		player->executeObjectControllerAction(STRING_HASHCODE("mount"), controlledObject->getObjectID(), ""); // Auto mount.
	}

	updateStatus(1);

	if (vehicleControlObserver != nullptr) {
		player->dropObserver(ObserverEventType::STARTCOMBAT, vehicleControlObserver);
	}
}

void VehicleControlDeviceImplementation::cancelSpawnObject(CreatureObject* player) {
	Reference<Task*> mountTask = player->getPendingTask("call_mount");
	if (mountTask) {
		mountTask->cancel();
		player->removePendingTask("call_mount");
	}

	if (vehicleControlObserver != nullptr)
		player->dropObserver(ObserverEventType::STARTCOMBAT, vehicleControlObserver);
}

void VehicleControlDeviceImplementation::storeObject(CreatureObject* player, bool force) {
	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject == nullptr)
		return;

	/*if (!controlledObject->isInQuadTree())
		return;*/

	if (!force && (player->isInCombat() || player->isDead()))
		return;

	if (player->isRidingMount() && player->getParent() == controlledObject) {
		if (!force && !player->checkCooldownRecovery("mount_dismount"))
			return;

		player->executeObjectControllerAction(STRING_HASHCODE("dismount"));

		if (player->isRidingMount())
			return;
	}

	Locker crossLocker(controlledObject, player);

	Reference<Task*> decayTask = controlledObject->getPendingTask("decay");

	if (decayTask != nullptr) {
		decayTask->cancel();
		controlledObject->removePendingTask("decay");
	}

	controlledObject->destroyObjectFromWorld(true);

	if (controlledObject->isCreatureObject())
		(cast<CreatureObject*>(controlledObject.get()))->setCreatureLink(nullptr);

	crossLocker.release();

	Locker deviceLocker(_this.getReferenceUnsafeStaticCast(), player);

	updateStatus(0);

	ManagedReference<VehicleObject*> vehicle = cast<VehicleObject*>(controlledObject.get());

	if (vehicle != nullptr && vehicle->isRentalVehicle() && vehicle->getRentalUses() <= 0) {
		destroyObjectFromWorld(true);
		destroyObjectFromDatabase(true);

		StringIdChatParameter param;
		param.setStringId("pet/pet_menu", "uses_complete");
		player->sendSystemMessage(param.toString());

		return;
	}
}

bool VehicleControlDeviceImplementation::recoverObject(CreatureObject* player) {
	ManagedReference<TangibleObject*> controlled = controlledObject.get();
	auto self = _this.getReferenceUnsafeStaticCast();

	auto reject = [&](const char* reason) {
		warning() << "Skipping vehicle recovery: " << reason
			<< " (player=" << (player != nullptr ? player->getObjectID() : 0)
			<< ", device=" << getObjectID()
			<< ", vehicle=" << (controlled != nullptr ? controlled->getObjectID() : 0) << ")";
		return false;
	};

	if (player == nullptr || controlled == nullptr || !controlled->isVehicleObject()) {
		return reject("missing player or valid vehicle");
	}

	ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");
	ManagedReference<VehicleObject*> vehicle = cast<VehicleObject*>(controlled.get());
	Locker vehicleLocker(vehicle, player);

	// The datapad's forward reference is authoritative only when all surviving
	// ownership links agree. Never assign a vehicle claimed by another device.
	auto validate = [&]() -> const char* {
		if (datapad == nullptr || player->getSlottedObject("datapad") != datapad ||
			datapad->getParent().get() != player || getParent().get() != datapad ||
			!datapad->hasObjectInContainer(getObjectID())) {
			return "device is not in the player's datapad";
		}

		if (controlledObject.get() != controlled) {
			return "controlled vehicle changed";
		}

		auto linkedDevice = vehicle->getControlDevice().get();
		auto linkedOwner = vehicle->getLinkedCreature().get();

		if (linkedDevice != nullptr && linkedDevice != self) {
			return "vehicle references another control device";
		}

		if (linkedOwner != nullptr && linkedOwner != player) {
			return "vehicle references another owner";
		}

		for (int i = 0; i < datapad->getContainerObjectsSize(); ++i) {
			auto other = datapad->getContainerObject(i);

			if (other == nullptr || other == self || !other->isControlDevice()) {
				continue;
			}

			auto otherDevice = cast<ControlDevice*>(other.get());

			if (otherDevice->getControlledObject() == vehicle) {
				return "multiple datapad devices reference the vehicle";
			}
		}

		if (vehicle->getParent() != nullptr) {
			return "vehicle has a parent";
		}

		// Do not dismount here: jetpack dismount calls normal storage, which can
		// delete rentals. Occupied or otherwise contained objects need review.
		if (vehicle->getContainerObjectsSize() != 0 || player->getParent().get() == vehicle) {
			return "vehicle is occupied or contains objects";
		}

		// Vehicle templates create a default unarmed weapon in this slot. It
		// belongs to the vehicle and must survive recovery along with it.
		auto defaultWeapon = vehicle->getSlottedObject("default_weapon");

		for (int i = 0; i < vehicle->getSlottedObjectsSize(); ++i) {
			auto child = vehicle->getSlottedObject(i);

			if (child == nullptr || child != defaultWeapon || !child->isWeaponObject() ||
				child->getParent().get() != vehicle) {
				return "vehicle has an occupant or unexpected slotted object";
			}
		}

		return nullptr;
	};

	if (auto reason = validate()) {
		return reject(reason);
	}

	bool changed = vehicle->getLocalZone() != nullptr || getStatus() != 0 ||
		vehicle->getLinkedCreature() != nullptr || vehicle->getControlDevice() == nullptr ||
		vehicle->hasState(CreatureState::MOUNTEDCREATURE);

	Reference<Task*> decayTask = vehicle->getPendingTask("decay");

	if (decayTask != nullptr) {
		decayTask->cancel();
		vehicle->removePendingTask("decay");
	}

	// Removing the existing object preserves its identity, damage, paint, and
	// rental uses. In particular, do not call spawnObject() or storeObject().
	if (vehicle->getLocalZone() != nullptr) {
		vehicle->destroyObjectFromWorld(true);
	}

	// World removal invokes observers. Check the relationships again before
	// repairing them or reporting a successfully stored vehicle.
	if (auto reason = validate()) {
		return reject(reason);
	}

	if (vehicle->getLocalZone() != nullptr || vehicle->isInQuadTree() || vehicle->isInOctree()) {
		return reject("vehicle could not be removed from the world");
	}

	vehicle->setControlDevice(self);
	vehicle->setCreatureLink(nullptr);
	vehicle->clearState(CreatureState::MOUNTEDCREATURE, false);
	updateStatus(0);

	if (changed) {
		info(false) << "Recovered vehicle to stored state (player=" << player->getObjectID()
			<< ", device=" << getObjectID() << ", vehicle=" << vehicle->getObjectID() << ")";
	}

	return true;
}

void VehicleControlDeviceImplementation::destroyObjectFromDatabase(bool destroyContainedObjects) {
	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject != nullptr) {
		Locker locker(controlledObject);

		ManagedReference<CreatureObject*> object = controlledObject->getSlottedObject("rider").castTo<CreatureObject*>();

		if (object != nullptr) {
			Locker clocker(object, controlledObject);

			object->executeObjectControllerAction(STRING_HASHCODE("dismount"));

			object = controlledObject->getSlottedObject("rider").castTo<CreatureObject*>();

			if (object != nullptr) {
				controlledObject->removeObject(object, nullptr, true);

				Zone* zone = getZone();

				if (zone != nullptr)
					zone->transferObject(object, -1, true);
			}
		}

		controlledObject->destroyObjectFromDatabase(true);
	}

	IntangibleObjectImplementation::destroyObjectFromDatabase(destroyContainedObjects);
}

int VehicleControlDeviceImplementation::canBeDestroyed(CreatureObject* player) {
	ManagedReference<TangibleObject*> controlledObject = this->controlledObject.get();

	if (controlledObject != nullptr) {
		if (controlledObject->isInQuadTree())
			return 1;
	}

	return IntangibleObjectImplementation::canBeDestroyed(player);
}

bool VehicleControlDeviceImplementation::canBeTradedTo(CreatureObject* player, CreatureObject* receiver, int numberInTrade) {
	ManagedReference<SceneObject*> datapad = receiver->getSlottedObject("datapad");

	if (datapad == nullptr)
		return false;

	ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

	int vehiclesInDatapad = numberInTrade;
	int maxStoredVehicles = playerManager->getBaseStoredVehicles();

	for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
		Reference<SceneObject*> obj = datapad->getContainerObject(i).castTo<SceneObject*>();

		if (obj != nullptr && obj->isVehicleControlDevice()) {
			vehiclesInDatapad++;
		}
	}

	if (vehiclesInDatapad >= maxStoredVehicles) {
		player->sendSystemMessage("That person has too many vehicles in their datapad");
		receiver->sendSystemMessage("@pet/pet_menu:has_max_vehicle"); // You already have the maximum number of vehicles that you can own.
		return false;
	}

	return true;
}

void VehicleControlDeviceImplementation::fillAttributeList(AttributeListMessage* alm, CreatureObject* object) {
	SceneObjectImplementation::fillAttributeList(alm, object);

	if (this->controlledObject == nullptr)
		return;

	ManagedReference<VehicleObject*> vehicle = this->controlledObject.get().castTo<VehicleObject*>();
	if (vehicle == nullptr)
		return;

	if (vehicle->getPaintCount() > 0) {
		alm->insertAttribute("customization_cnt", vehicle->getPaintCount());
	}
}
