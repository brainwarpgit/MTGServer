#ifndef VEHICLEJUMP_H_
#define VEHICLEJUMP_H_

#include "server/zone/objects/creature/VehicleObject.h"
#include "templates/params/creature/CreaturePosture.h"

namespace VehicleJump {

// Callers hold both the vehicle and player locks.
inline bool canJump(VehicleObject* vehicle, CreatureObject* player) {
	if (vehicle == nullptr || player == nullptr || !player->isPlayerCreature() ||
			vehicle->getClientObjectCRC() != STRING_HASHCODE("object/mobile/vehicle/shared_grievous_wheel_bike.iff")) {
		return false;
	}

	if (vehicle->getLinkedCreature().get() != player || !player->isRidingMount() ||
			!vehicle->hasRidingCreature() || player->getParent().get() != vehicle ||
			vehicle->getSlottedObject("rider") != player) {
		return false;
	}

	return vehicle->getZone() != nullptr && vehicle->getZone() == player->getZone() &&
			!vehicle->isDisabled() && vehicle->getPosture() == CreaturePosture::UPRIGHT &&
			player->getPosture() == CreaturePosture::UPRIGHT;
}

// A valid mounted jump is handled even while its animation is on cooldown.
inline bool handleJump(VehicleObject* vehicle, CreatureObject* player) {
	if (!canJump(vehicle, player))
		return false;

	if (vehicle->checkCooldownRecovery("wheelbike_jump")) {
		// Send the authored hop animation; the client controls its motion.
		vehicle->updateCooldownTimer("wheelbike_jump", 2000);
		vehicle->doAnimation("jump");
	}

	return true;
}

} // namespace VehicleJump

#endif /* VEHICLEJUMP_H_ */
