#include "gtest/gtest.h"

#include "server/zone/objects/creature/VehicleObject.h"
#include "server/zone/objects/creature/events/VehicleDecayTask.h"

namespace {

class DecayTestVehicleTemplate : public VehicleObjectTemplate {
public:
	DecayTestVehicleTemplate() {
		setMaxCondition(1000);
		setTargetable(true);

		for (int i = 0; i < 9; ++i) {
			baseHAM.add(1000);
		}
	}
};

TEST(VehicleDecayTaskTest, AlreadyDequeuedTaskDoesNotDamageOrRescheduleStoredVehicle) {
	Reference<VehicleObjectTemplate*> vehicleTemplate = new DecayTestVehicleTemplate();
	Reference<VehicleObject*> vehicle = new VehicleObject();

	{
		Locker locker(vehicle);
		vehicle->_setObjectID(1);
		vehicle->loadTemplateData(vehicleTemplate);
		vehicle->setConditionDamage(73, false);
	}

	ASSERT_TRUE(vehicle->getLocalZone() == nullptr);
	ASSERT_TRUE(vehicle->getObjectTemplate() != nullptr);
	Reference<VehicleDecayTask*> task = new VehicleDecayTask(vehicle);

	// A valid decay template ensures the test exercises the stored-state
	// guard instead of returning early because template data is absent.
	task->run();

	Locker locker(vehicle);
	EXPECT_EQ(vehicle->getConditionDamage(), 73);
	Reference<Task*> pending = vehicle->getPendingTask("decay");
	EXPECT_TRUE(pending == nullptr);

	// Keep a failing regression from leaving a scheduled task behind.
	if (pending != nullptr) {
		pending->cancel();
		vehicle->removePendingTask("decay");
	}
}

} // namespace
