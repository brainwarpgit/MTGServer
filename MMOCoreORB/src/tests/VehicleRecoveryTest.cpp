#include "gtest/gtest.h"

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/VehicleObject.h"
#include "server/zone/objects/intangible/VehicleControlDevice.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/objects/tangible/weapon/WeaponObject.h"

namespace {

class VehicleRecoveryTest : public ::testing::Test {
protected:
	Reference<CreatureObject*> player;
	Reference<SceneObject*> datapad;
	Reference<VehicleControlDevice*> device;
	Reference<VehicleObject*> vehicle;
	uint64 nextObjectID = 100;

	template<class Object>
	Reference<Object*> makeObject() {
		Reference<Object*> object = new Object();
		object->_setObjectID(++nextObjectID);
		object->initializeContainerObjectsMap();
		return object;
	}

	void SetUp() override {
		// Populate the same parent, slot and container links restored from the
		// database, without templates, a zone, a database or the task manager.
		player = makeObject<CreatureObject>();
		datapad = makeObject<SceneObject>();
		device = makeObject<VehicleControlDevice>();
		vehicle = makeObject<VehicleObject>();

		player->getSlottedObjects()->put("datapad", datapad);
		datapad->setParent(player, false);
		datapad->putInContainer(device, device->getObjectID());
		device->setParent(datapad, false);
		device->setControlledObject(vehicle);
		device->updateStatus(1, false);
		vehicle->setCreatureLink(player, false);
	}

	bool recover() {
		Locker playerLocker(player);
		Locker deviceLocker(device, player);
		return device->recoverObject(player);
	}

	void expectStored() {
		ManagedReference<ControlDevice*> restoredDevice = vehicle->getControlDevice().get();
		ManagedReference<TangibleObject*> controlled = device->getControlledObject();
		EXPECT_EQ(device.get(), restoredDevice.get());
		EXPECT_EQ(vehicle.get(), controlled.get());
		EXPECT_TRUE(vehicle->getLinkedCreature().get() == nullptr);
		EXPECT_TRUE(vehicle->getLocalZone() == nullptr);
		EXPECT_TRUE(vehicle->getParent().get() == nullptr);
		EXPECT_FALSE(vehicle->isInQuadTree());
		EXPECT_EQ(0u, device->getStatus());
		EXPECT_TRUE(datapad->hasObjectInContainer(device->getObjectID()));
	}

	void expectRejectedWithoutChangingLinks() {
		ManagedReference<ControlDevice*> originalDevice = vehicle->getControlDevice().get();
		ManagedReference<CreatureObject*> originalOwner = vehicle->getLinkedCreature().get();
		ManagedReference<TangibleObject*> originalControlled = device->getControlledObject();
		const auto originalStatus = device->getStatus();

		EXPECT_FALSE(recover());

		ManagedReference<ControlDevice*> remainingDevice = vehicle->getControlDevice().get();
		ManagedReference<CreatureObject*> remainingOwner = vehicle->getLinkedCreature().get();
		ManagedReference<TangibleObject*> remainingControlled = device->getControlledObject();
		EXPECT_EQ(originalDevice.get(), remainingDevice.get());
		EXPECT_EQ(originalOwner.get(), remainingOwner.get());
		EXPECT_EQ(originalControlled.get(), remainingControlled.get());
		EXPECT_EQ(originalStatus, device->getStatus());
	}

	void TearDown() override {
		vehicle->getSlottedObjects()->removeAll();
		vehicle->getChildObjects()->removeAll();
		vehicle->removeAllContainerObjects();
		vehicle->setParent(nullptr, false);
		vehicle->setCreatureLink(nullptr, false);
		vehicle->setControlDevice(nullptr);
		device->setControlledObject(nullptr);
		device->setParent(nullptr, false);
		datapad->removeAllContainerObjects();
		datapad->setParent(nullptr, false);
		player->getSlottedObjects()->removeAll();
	}
};

TEST_F(VehicleRecoveryTest, RepairsMissingBacklinkAndClearsStaleOwnerAndStatus) {
	ASSERT_TRUE(vehicle->getControlDevice().get() == nullptr);
	ASSERT_TRUE(recover());
	expectStored();
}

TEST_F(VehicleRecoveryTest, AcceptsConsistentBacklink) {
	vehicle->setControlDevice(device);

	ASSERT_TRUE(recover());
	expectStored();
}

TEST_F(VehicleRecoveryTest, AcceptsMissingOwnerLink) {
	vehicle->setCreatureLink(nullptr, false);

	ASSERT_TRUE(recover());
	expectStored();
}

TEST_F(VehicleRecoveryTest, ClearsStaleMountedStateOnEmptyVehicle) {
	vehicle->setState(CreatureState::MOUNTEDCREATURE, false);

	ASSERT_TRUE(recover());
	expectStored();
	EXPECT_FALSE(vehicle->hasState(CreatureState::MOUNTEDCREATURE));
}

TEST_F(VehicleRecoveryTest, PreservesDefaultWeaponCreatedByVehicleTemplate) {
	auto weapon = makeObject<WeaponObject>();
	weapon->setParent(vehicle, false);
	vehicle->getSlottedObjects()->put("default_weapon", ManagedReference<SceneObject*>(weapon.get()));
	vehicle->addChildObject(weapon);

	ASSERT_TRUE(recover());
	expectStored();
	EXPECT_TRUE(vehicle->getSlottedObject("default_weapon") == weapon);
	EXPECT_TRUE(vehicle->containsChildObject(weapon));
	EXPECT_TRUE(weapon->getParent().get() == vehicle);
}

TEST_F(VehicleRecoveryTest, RejectsRiderInDefaultWeaponSlot) {
	auto rider = makeObject<CreatureObject>();
	rider->setParent(vehicle, false);
	vehicle->getSlottedObjects()->put("default_weapon", ManagedReference<SceneObject*>(rider.get()));

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RepeatedRecoveryKeepsSameVehicleAndDevice) {
	const auto vehicleID = vehicle->getObjectID();
	const auto deviceID = device->getObjectID();

	ASSERT_TRUE(recover());
	ASSERT_TRUE(recover());

	expectStored();
	EXPECT_EQ(vehicleID, vehicle->getObjectID());
	EXPECT_EQ(deviceID, device->getObjectID());
}

TEST_F(VehicleRecoveryTest, PreservesExpiredRentalConditionPaintAndCustomization) {
	vehicle->setRentalVehicle(true);
	vehicle->setUses(0);
	vehicle->setMaxCondition(1000, false);
	vehicle->setConditionDamage(275, false);
	vehicle->refreshPaint();
	vehicle->setCustomizationVariable(static_cast<byte>(1), 42, false);

	String originalCustomization;
	vehicle->getCustomizationString(originalCustomization);
	const auto originalPaintCount = vehicle->getPaintCount();
	const auto vehicleID = vehicle->getObjectID();
	const auto deviceID = device->getObjectID();

	ASSERT_TRUE(recover());

	String remainingCustomization;
	vehicle->getCustomizationString(remainingCustomization);
	expectStored();
	EXPECT_EQ(vehicleID, vehicle->getObjectID());
	EXPECT_EQ(deviceID, device->getObjectID());
	EXPECT_TRUE(vehicle->isRentalVehicle());
	EXPECT_EQ(0, vehicle->getRentalUses());
	EXPECT_EQ(1000, vehicle->getMaxCondition());
	EXPECT_EQ(275, vehicle->getConditionDamage());
	EXPECT_EQ(originalPaintCount, vehicle->getPaintCount());
	EXPECT_TRUE(originalCustomization == remainingCustomization);
}

TEST_F(VehicleRecoveryTest, RejectsMissingDatapadSlot) {
	player->getSlottedObjects()->drop("datapad");

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsDatapadBelongingToAnotherCreature) {
	auto otherPlayer = makeObject<CreatureObject>();
	datapad->setParent(otherPlayer, false);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsDeviceOutsideDatapadDespiteStaleContainerEntry) {
	auto otherContainer = makeObject<SceneObject>();
	device->setParent(otherContainer, false);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsDeviceAbsentFromDatapadContainer) {
	datapad->removeAllContainerObjects();

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsMissingControlledObject) {
	device->setControlledObject(nullptr);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsNonVehicleControlledObject) {
	auto unrelatedObject = makeObject<TangibleObject>();
	device->setControlledObject(unrelatedObject);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsVehicleLinkedToAnotherCreature) {
	auto otherPlayer = makeObject<CreatureObject>();
	vehicle->setCreatureLink(otherPlayer, false);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsVehicleLinkedToAnotherControlDevice) {
	auto otherDevice = makeObject<VehicleControlDevice>();
	vehicle->setControlDevice(otherDevice);

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, RejectsTwoDatapadDevicesClaimingSameVehicle) {
	// Even a non-vehicle control device is a conflicting forward claim.
	auto otherDevice = makeObject<ControlDevice>();
	otherDevice->setControlledObject(vehicle);
	otherDevice->setParent(datapad, false);
	datapad->putInContainer(otherDevice, otherDevice->getObjectID());

	expectRejectedWithoutChangingLinks();
}

TEST_F(VehicleRecoveryTest, IgnoresUnrelatedVehicleInSameDatapad) {
	auto otherDevice = makeObject<VehicleControlDevice>();
	auto otherVehicle = makeObject<VehicleObject>();
	otherDevice->setControlledObject(otherVehicle);
	otherDevice->setParent(datapad, false);
	datapad->putInContainer(otherDevice, otherDevice->getObjectID());

	ASSERT_TRUE(recover());
	expectStored();
	ManagedReference<TangibleObject*> otherControlled = otherDevice->getControlledObject();
	EXPECT_EQ(otherVehicle.get(), otherControlled.get());
	EXPECT_TRUE(datapad->hasObjectInContainer(otherDevice->getObjectID()));
}

TEST_F(VehicleRecoveryTest, RejectsParentedVehicle) {
	auto parent = makeObject<SceneObject>();
	vehicle->setParent(parent, false);

	expectRejectedWithoutChangingLinks();
	EXPECT_TRUE(vehicle->getParent().get() == parent);
}

TEST_F(VehicleRecoveryTest, RejectsSlottedRider) {
	auto rider = makeObject<CreatureObject>();
	rider->setParent(vehicle, false);
	vehicle->getSlottedObjects()->put("rider", ManagedReference<SceneObject*>(rider.get()));

	expectRejectedWithoutChangingLinks();
	EXPECT_TRUE(rider->getParent().get() == vehicle);
	EXPECT_TRUE(vehicle->getSlottedObject("rider") == rider);
}

TEST_F(VehicleRecoveryTest, RejectsContainedPassenger) {
	auto passenger = makeObject<CreatureObject>();
	passenger->setParent(vehicle, false);
	vehicle->putInContainer(passenger, passenger->getObjectID());

	expectRejectedWithoutChangingLinks();
	EXPECT_TRUE(passenger->getParent().get() == vehicle);
	EXPECT_TRUE(vehicle->hasObjectInContainer(passenger->getObjectID()));
}

TEST_F(VehicleRecoveryTest, RejectsMountedOwnerWithMissingRiderEntry) {
	player->setParent(vehicle, false);

	expectRejectedWithoutChangingLinks();
	EXPECT_TRUE(player->getParent().get() == vehicle);
	player->setParent(nullptr, false);
}

} // namespace
