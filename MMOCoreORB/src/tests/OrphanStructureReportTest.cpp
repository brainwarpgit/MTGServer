#include "gtest/gtest.h"

#include "server/zone/managers/structure/OrphanStructureReport.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "templates/creature/PlayerCreatureTemplate.h"

#include <initializer_list>

namespace {

TEST(OrphanStructureReportTest, DefaultsToAllPlanetsAndFirstPage) {
	for (const char* arguments : {"", "   ", "all", "ALL"}) {
		String planet = "previous";
		int page = 99;
		ASSERT_TRUE(OrphanStructureReport::parseArguments(arguments, planet, page));
		EXPECT_TRUE(planet == "all");
		EXPECT_EQ(1, page);
	}
}

TEST(OrphanStructureReportTest, NormalizesPlanetAndAcceptsRequestedPage) {
	String planet;
	int page;
	ASSERT_TRUE(OrphanStructureReport::parseArguments("  Tatooine  12  ", planet, page));
	EXPECT_TRUE(planet == "tatooine");
	EXPECT_EQ(12, page);

	ASSERT_TRUE(OrphanStructureReport::parseArguments("naboo", planet, page));
	EXPECT_TRUE(planet == "naboo");
	EXPECT_EQ(1, page);

	ASSERT_TRUE(OrphanStructureReport::parseArguments("all 2", planet, page));
	EXPECT_TRUE(planet == "all");
	EXPECT_EQ(2, page);
}

TEST(OrphanStructureReportTest, AcceptsPositiveNumericPageBoundaries) {
	String planet;
	int page;
	ASSERT_TRUE(OrphanStructureReport::parseArguments("all 000001", planet, page));
	EXPECT_EQ(1, page);
	ASSERT_TRUE(OrphanStructureReport::parseArguments("all 999999", planet, page));
	EXPECT_EQ(999999, page);
}

TEST(OrphanStructureReportTest, RejectsZeroNegativeAndNonNumericPages) {
	for (const char* arguments : {"all 0", "all 000000", "all -1", "all +1", "all 1.5", "all one", "all 1x"}) {
		String planet;
		int page;
		EXPECT_FALSE(OrphanStructureReport::parseArguments(arguments, planet, page)) << arguments;
	}
}

TEST(OrphanStructureReportTest, RejectsOversizedPagesBeforeIntegerOverflow) {
	for (const char* arguments : {"all 1000000", "all 2147483648", "all 18446744073709551616"}) {
		String planet;
		int page;
		EXPECT_FALSE(OrphanStructureReport::parseArguments(arguments, planet, page)) << arguments;
	}
}

TEST(OrphanStructureReportTest, RejectsTrailingArguments) {
	String planet;
	int page;
	EXPECT_FALSE(OrphanStructureReport::parseArguments("tatooine 1 extra", planet, page));
	EXPECT_FALSE(OrphanStructureReport::parseArguments("all 1 2", planet, page));
}

TEST(OrphanStructureReportTest, HelpArgumentsRequestUsage) {
	for (const char* arguments : {"help", "HELP", "?", "help 2"}) {
		String planet;
		int page;
		EXPECT_FALSE(OrphanStructureReport::parseArguments(arguments, planet, page)) << arguments;
	}
}

TEST(OrphanStructureReportTest, RejectsNullRequester) {
	EXPECT_EQ(1, OrphanStructureReport::execute(nullptr, "all"));
}

TEST(OrphanStructureReportTest, RejectsNonPlayerRequesterWithoutSchedulingScan) {
	Reference<CreatureObject*> creature = new CreatureObject();
	ASSERT_FALSE(creature->isPlayerCreature());
	EXPECT_EQ(1, OrphanStructureReport::execute(creature, "all"));
	EXPECT_TRUE(creature->getPendingTask("orphanStructureReport") == nullptr);
}

TEST(OrphanStructureReportTest, RejectsPlayerBelowAdministratorLevelWithoutSchedulingScan) {
	Reference<CreatureObject*> player = new CreatureObject();
	Reference<PlayerCreatureTemplate*> playerTemplate = new PlayerCreatureTemplate();
	playerTemplate->setCollisionActionBlockFlags(0);
	// Qualify the servant's base implementation directly: qualifying the stub
	// would still dispatch to CreatureObjectImplementation::loadTemplateData.
	// The permission check needs scene identity, not HAM or server callbacks.
	auto implementation = static_cast<CreatureObjectImplementation*>(player->_getImplementation());
	implementation->SceneObjectImplementation::loadTemplateData(playerTemplate.get());
	ASSERT_TRUE(player->isPlayerCreature());
	ASSERT_TRUE(player->getZoneServer() == nullptr);

	ManagedReference<PlayerObject*> ghost = new PlayerObject();
	ManagedReference<SceneObject*> ghostSlot = ghost.get();
	player->getSlottedObjects()->put("ghost", ghostSlot);
	ASSERT_EQ(player->getPlayerObject().get(), ghost.get());

	for (unsigned int adminLevel : {0u, 14u}) {
		ghost->setAdminLevel(adminLevel);
		EXPECT_EQ(1, OrphanStructureReport::execute(player, "all"));
		EXPECT_TRUE(player->getPendingTask("orphanStructureReport") == nullptr);
	}
	player->getSlottedObjects()->removeAll();
}

} // namespace
