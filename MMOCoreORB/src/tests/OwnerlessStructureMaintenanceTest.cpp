#include "gtest/gtest.h"

#include "server/zone/objects/structure/StructureObject.h"
#include "server/zone/objects/structure/StructureOwnership.h"
#include "templates/tangible/SharedStructureObjectTemplate.h"

#include <string>

namespace {

struct StructureOwnershipState {
	uint64 objectID = (uint64(1) << 48) | 123;
	uint64 ownerID = 0;
	bool persistent = true;
	bool clientObject = false;
	bool civic = false;

	// Match the generated TreeEntry API: this read accessor is not const.
	uint64 getObjectID() { return objectID; }
	uint64 getOwnerObjectID() const { return ownerID; }
	bool isPersistent() const { return persistent; }
	bool isClientObject() const { return clientObject; }
	bool isCivicStructure() const { return civic; }
};

class MaintenanceTestTemplate : public SharedStructureObjectTemplate {
public:
	MaintenanceTestTemplate() {
		baseMaintenanceRate = 100;
	}
};

struct StatusUpdateReached {};

class MaintenanceTestStructure : public StructureObjectImplementation {
public:
	std::string logMessages;

	MaintenanceTestStructure() {
		templateObject = new MaintenanceTestTemplate();
		setLoggerCallback([this](Logger::LogLevel, const char* message) -> int {
			logMessages += message;
			return Logger::SUCCESS;
		});
	}

	~MaintenanceTestStructure() override {
		clearLoggerCallback();
	}

	void updateStructureStatus() override {
		// Stop at the first maintenance side effect to exercise the real
		// scheduler without running a task or accessing a database.
		throw StatusUpdateReached{};
	}
};

TEST(OwnerlessStructureMaintenanceTest, PersistedPlayerStructureWithoutOwnerStillRequiresMaintenance) {
	StructureOwnershipState structure;
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, ClearingPersistedPlayerOwnerDoesNotDisableMaintenance) {
	StructureOwnershipState structure;
	structure.ownerID = 456;
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(&structure));

	structure.ownerID = 0;
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, WorldSnapshotGeneratorDoesNotRequirePlayerMaintenance) {
	StructureOwnershipState structure;
	structure.objectID = 609457821;
	// Even if an old initialization path cleared the client flag, the world
	// namespace must keep this NPC installation out of player maintenance.
	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, OwnerlessTemporaryQuestStructureDoesNotRequireMaintenance) {
	StructureOwnershipState structure;
	structure.persistent = false;
	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, OwnerlessClientStructureDoesNotRequireMaintenance) {
	StructureOwnershipState structure;
	structure.clientObject = true;
	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, OwnerlessCivicStructureDoesNotRequirePlayerMaintenance) {
	StructureOwnershipState structure;
	structure.civic = true;
	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, AssignedOwnerStillRequiresMaintenance) {
	StructureOwnershipState structure;
	structure.ownerID = 456;
	structure.persistent = false;
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(&structure));
}

TEST(OwnerlessStructureMaintenanceTest, GeneratedStubAndServantSupportOwnershipChecks) {
	Reference<StructureObject*> structure = new StructureObject();
	structure->_setObjectID(609457821);
	structure->setPersistent(1);
	auto implementation = static_cast<StructureObjectImplementation*>(structure->_getImplementation());

	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(structure.get()));
	EXPECT_FALSE(StructureOwnership::requiresPlayerMaintenance(implementation));

	structure->_setObjectID((uint64(1) << 48) | 123);
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(structure.get()));
	EXPECT_TRUE(StructureOwnership::requiresPlayerMaintenance(implementation));
}

TEST(OwnerlessStructureMaintenanceTest, ConstStatusSupportsOwnerlessWorldStructure) {
	Reference<StructureObject*> structure = new StructureObject();
	structure->_setObjectID(609457821);
	structure->setPersistent(1);
	const auto* implementation = static_cast<StructureObjectImplementation*>(structure->_getImplementation());

	EXPECT_TRUE(implementation->getDebugStructureStatus().isEmpty());
}

TEST(OwnerlessStructureMaintenanceTest, OwnedStructureStillUpdatesMaintenance) {
	MaintenanceTestStructure structure;
	structure.setOwner(123);
	ASSERT_GT(structure.getMaintenanceRate(), 0);
	EXPECT_THROW(structure.scheduleMaintenanceExpirationEvent(), StatusUpdateReached);
}

TEST(OwnerlessStructureMaintenanceTest, OwnedStructureStillReportsMissingTask) {
	MaintenanceTestStructure structure;
	structure.setOwner(123);
	ASSERT_GT(structure.getBaseMaintenanceRate(), 0);

	EXPECT_TRUE(structure.getDebugStructureStatus().contains("No maintenance task running"));
	EXPECT_NE(std::string::npos, structure.logMessages.find("structureMaintenanceTask == nullptr"));
}

} // namespace
