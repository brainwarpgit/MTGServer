#include "gtest/gtest.h"

#include "server/zone/managers/skill/SkillSurrenderPlan.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace {

class SurrenderTestSkill : public Skill {
public:
	SurrenderTestSkill(const String& name, int points = 2) {
		skillName = name;
		pointsRequired = points;
		// Ordinary trainable boxes can also confer a title.
		title = true;
	}

	void require(Skill& skill) {
		skillsRequired.add(skill.getSkillName());
		skillsRequiredCount = skillsRequired.size();
	}

	void makeHidden() { hidden = true; }
	void makeGodOnly() { godOnly = true; }
	void makeProfessionRoot() { profession = true; }
};

void expectBefore(const std::vector<Skill*>& plan, Skill* dependent, Skill* prerequisite) {
	const auto dependentPosition = std::find(plan.begin(), plan.end(), dependent);
	const auto prerequisitePosition = std::find(plan.begin(), plan.end(), prerequisite);
	ASSERT_NE(plan.end(), dependentPosition);
	ASSERT_NE(plan.end(), prerequisitePosition);
	EXPECT_LT(std::distance(plan.begin(), dependentPosition), std::distance(plan.begin(), prerequisitePosition));
}

TEST(SkillSurrenderPlanTest, NoviceEntertainerIncludesAllOwnedBranchesAndDependentProfessions) {
	SurrenderTestSkill novice("social_entertainer_novice", 15);
	SurrenderTestSkill dancing1("social_entertainer_dance_01");
	SurrenderTestSkill dancing2("social_entertainer_dance_02");
	SurrenderTestSkill music("social_entertainer_music_01");
	SurrenderTestSkill master("social_entertainer_master", 1);
	SurrenderTestSkill dancer("social_dancer_novice", 6);
	SurrenderTestSkill musician("social_musician_novice", 6);
	SurrenderTestSkill hairstyle("social_entertainer_hairstyle_04");
	SurrenderTestSkill imageDesigner("social_imagedesigner_novice", 6);
	SurrenderTestSkill unrelated("crafting_artisan_novice", 15);
	dancing1.require(novice);
	dancing2.require(dancing1);
	music.require(novice);
	master.require(dancing2);
	master.require(music);
	dancer.require(dancing2);
	musician.require(music);
	hairstyle.require(novice);
	imageDesigner.require(hairstyle);

	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build(
		{&unrelated, &novice, &music, &dancer, &dancing2, &master, &musician, &dancing1, &imageDesigner, &hairstyle}, &novice, plan));
	ASSERT_EQ(9u, plan.size());
	EXPECT_EQ(&novice, plan.back());
	EXPECT_EQ(plan.end(), std::find(plan.begin(), plan.end(), &unrelated));
	expectBefore(plan, &dancing1, &novice);
	expectBefore(plan, &dancing2, &dancing1);
	expectBefore(plan, &music, &novice);
	expectBefore(plan, &master, &dancing2);
	expectBefore(plan, &master, &music);
	expectBefore(plan, &dancer, &dancing2);
	expectBefore(plan, &musician, &music);
	expectBefore(plan, &hairstyle, &novice);
	expectBefore(plan, &imageDesigner, &hairstyle);
}

TEST(SkillSurrenderPlanTest, MidTierLeavesLowerSkillsAndOtherBranchesUntouched) {
	SurrenderTestSkill novice("social_entertainer_novice", 15);
	SurrenderTestSkill first("social_entertainer_dance_01");
	SurrenderTestSkill selected("social_entertainer_dance_02");
	SurrenderTestSkill higher("social_entertainer_dance_03");
	SurrenderTestSkill music("social_entertainer_music_01");
	first.require(novice);
	selected.require(first);
	higher.require(selected);
	music.require(novice);

	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&novice, &first, &selected, &higher, &music}, &selected, plan));
	EXPECT_EQ((std::vector<Skill*>{&higher, &selected}), plan);
}

TEST(SkillSurrenderPlanTest, RejectsNullAndUnownedSelectedSkillsAndClearsPreviousPlan) {
	SurrenderTestSkill owned("social_entertainer_novice");
	SurrenderTestSkill unowned("social_dancer_novice");
	SurrenderTestSkill duplicateName("social_entertainer_novice");
	for (Skill* selected : std::vector<Skill*>{nullptr, &unowned, &duplicateName}) {
		std::vector<Skill*> plan{&owned};
		EXPECT_FALSE(SkillSurrenderPlan::build({&owned}, selected, plan));
		EXPECT_TRUE(plan.empty());
	}
}

TEST(SkillSurrenderPlanTest, RemovesSharedDependentOnceAndBeforeBothPrerequisites) {
	SurrenderTestSkill novice("social_entertainer_novice");
	SurrenderTestSkill dance("social_entertainer_dance_01");
	SurrenderTestSkill music("social_entertainer_music_01");
	SurrenderTestSkill master("social_entertainer_master");
	dance.require(novice);
	music.require(novice);
	master.require(dance);
	master.require(music);
	master.require(music);

	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&novice, &dance, &master, &music, &master, &novice, nullptr}, &novice, plan));
	ASSERT_EQ(4u, plan.size());
	EXPECT_EQ(1, std::count(plan.begin(), plan.end(), &master));
	expectBefore(plan, &master, &dance);
	expectBefore(plan, &master, &music);
	expectBefore(plan, &dance, &novice);
	expectBefore(plan, &music, &novice);
}

TEST(SkillSurrenderPlanTest, RemovalOrderDoesNotDependOnOwnedListOrder) {
	SurrenderTestSkill root("social_entertainer_novice");
	SurrenderTestSkill dance("social_entertainer_dance_01");
	SurrenderTestSkill music("social_entertainer_music_01");
	dance.require(root);
	music.require(root);
	std::vector<Skill*> first;
	std::vector<Skill*> second;
	ASSERT_TRUE(SkillSurrenderPlan::build({&root, &music, &dance}, &root, first));
	ASSERT_TRUE(SkillSurrenderPlan::build({&dance, &root, &music}, &root, second));
	EXPECT_EQ(first, second);
	EXPECT_EQ((std::vector<Skill*>{&dance, &music, &root}), first);
}

TEST(SkillSurrenderPlanTest, RejectsReachableCycleAndDiscardsAlreadyPlannedSkills) {
	SurrenderTestSkill selected("social_entertainer_novice");
	SurrenderTestSkill early("a_valid_dependent");
	SurrenderTestSkill first("z_cycle_first");
	SurrenderTestSkill second("z_cycle_second");
	early.require(selected);
	first.require(selected);
	first.require(second);
	second.require(first);

	std::vector<Skill*> plan;
	EXPECT_FALSE(SkillSurrenderPlan::build({&selected, &early, &first, &second}, &selected, plan));
	EXPECT_TRUE(plan.empty());
}

TEST(SkillSurrenderPlanTest, RejectsSelfDependency) {
	SurrenderTestSkill selected("social_entertainer_novice");
	selected.require(selected);
	std::vector<Skill*> plan;
	EXPECT_FALSE(SkillSurrenderPlan::build({&selected}, &selected, plan));
	EXPECT_TRUE(plan.empty());
}

TEST(SkillSurrenderPlanTest, IgnoresUnrelatedCycle) {
	SurrenderTestSkill selected("social_entertainer_novice");
	SurrenderTestSkill first("unrelated_first");
	SurrenderTestSkill second("unrelated_second");
	first.require(second);
	second.require(first);
	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&selected, &first, &second}, &selected, plan));
	EXPECT_EQ((std::vector<Skill*>{&selected}), plan);
}

TEST(SkillSurrenderPlanTest, RejectsProtectedSelectedSkillsAndProtectedDependents) {
	SurrenderTestSkill selected("social_entertainer_novice");
	SurrenderTestSkill earlier("a_valid_dependent");
	SurrenderTestSkill hidden("hidden_skill");
	SurrenderTestSkill godOnly("admin_skill");
	SurrenderTestSkill profession("social_entertainer");
	SurrenderTestSkill pilot("pilot_rebel_navy_novice");
	SurrenderTestSkill forceTitle("force_title_jedi_rank_01");
	SurrenderTestSkill forceRank("force_rank_light_novice");
	SurrenderTestSkill language("social_language_basic", 0);
	hidden.makeHidden();
	godOnly.makeGodOnly();
	profession.makeProfessionRoot();
	earlier.require(selected);

	for (auto protectedSkill : std::vector<Skill*>{&hidden, &godOnly, &profession, &pilot, &forceTitle, &forceRank, &language}) {
		auto testSkill = static_cast<SurrenderTestSkill*>(protectedSkill);
		testSkill->require(selected);
		EXPECT_FALSE(SkillSurrenderPlan::isSelectable(protectedSkill));
		std::vector<Skill*> plan{&selected};
		EXPECT_FALSE(SkillSurrenderPlan::build({&selected, protectedSkill}, protectedSkill, plan));
		EXPECT_TRUE(plan.empty());
		EXPECT_FALSE(SkillSurrenderPlan::build({&selected, &earlier, protectedSkill}, &selected, plan));
		EXPECT_TRUE(plan.empty());
	}
}

TEST(SkillSurrenderPlanTest, IgnoresUnrelatedProtectedSkills) {
	SurrenderTestSkill selected("social_entertainer_novice");
	SurrenderTestSkill hidden("hidden_skill");
	hidden.makeHidden();
	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&selected, &hidden}, &selected, plan));
	EXPECT_EQ((std::vector<Skill*>{&selected}), plan);
}

TEST(SkillSurrenderPlanTest, OrdinaryTrainableBoxesRemainSelectableWhenTheyGrantTitles) {
	SurrenderTestSkill novice("social_entertainer_novice", 15);
	ASSERT_TRUE(novice.isTitle());
	ASSERT_FALSE(novice.isSkill());
	EXPECT_TRUE(SkillSurrenderPlan::isSelectable(&novice));
}

TEST(SkillSurrenderPlanTest, IncludesZeroPointForceSensitiveNoviceAndMaster) {
	SurrenderTestSkill novice("force_sensitive_combat_prowess_novice", 0);
	SurrenderTestSkill trained("force_sensitive_combat_prowess_ranged_accuracy_01", 1);
	SurrenderTestSkill master("force_sensitive_combat_prowess_master", 0);
	trained.require(novice);
	master.require(trained);
	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&novice, &trained, &master}, &novice, plan));
	EXPECT_EQ((std::vector<Skill*>{&master, &trained, &novice}), plan);
}

TEST(SkillSurrenderPlanTest, DoesNotTreatEveryZeroPointForceSensitiveMarkerAsTrainable) {
	SurrenderTestSkill marker("force_sensitive_progression_marker", 0);
	SurrenderTestSkill negative("force_sensitive_combat_prowess_novice", -1);
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&marker));
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&negative));
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(nullptr));
}

TEST(SkillSurrenderPlanTest, IncludesZeroPointPoliticianBoxesButNotSpeciesAndLanguages) {
	SurrenderTestSkill novice("social_politician_novice", 0);
	SurrenderTestSkill fiscal("social_politician_fiscal_01", 0);
	SurrenderTestSkill master("social_politician_master", 0);
	SurrenderTestSkill species("species_human", 0);
	SurrenderTestSkill language("social_language_basic", 0);
	fiscal.require(novice);
	master.require(fiscal);
	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&novice, &fiscal, &master, &species, &language}, &novice, plan));
	EXPECT_EQ((std::vector<Skill*>{&master, &fiscal, &novice}), plan);
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&species));
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&language));
}

TEST(SkillSurrenderPlanTest, AdministrativePilotRemovalIncludesEveryOwnedDependentBeforePrerequisites) {
	SurrenderTestSkill novice("pilot_rebel_navy_novice", 0);
	SurrenderTestSkill starships1("pilot_rebel_navy_starships_01", 0);
	SurrenderTestSkill starships2("pilot_rebel_navy_starships_02", 0);
	SurrenderTestSkill weapons("pilot_rebel_navy_weapons_01", 0);
	SurrenderTestSkill master("pilot_rebel_navy_master", 0);
	SurrenderTestSkill unrelated("social_entertainer_novice", 15);
	starships1.require(novice);
	starships2.require(starships1);
	weapons.require(novice);
	master.require(starships2);
	master.require(weapons);

	std::vector<Skill*> plan;
	ASSERT_TRUE(SkillSurrenderPlan::build({&master, &starships2, &novice, &weapons, &unrelated, &starships1}, &novice, plan, true));
	ASSERT_EQ(5u, plan.size());
	EXPECT_EQ(&novice, plan.back());
	EXPECT_EQ(plan.end(), std::find(plan.begin(), plan.end(), &unrelated));
	expectBefore(plan, &starships1, &novice);
	expectBefore(plan, &starships2, &starships1);
	expectBefore(plan, &weapons, &novice);
	expectBefore(plan, &master, &starships2);
	expectBefore(plan, &master, &weapons);
}

TEST(SkillSurrenderPlanTest, PilotRemovalRequiresExplicitAdministrativeMode) {
	SurrenderTestSkill novice("pilot_neutral_novice", 0);
	SurrenderTestSkill higher("pilot_neutral_starships_01", 0);
	higher.require(novice);
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&novice));
	EXPECT_FALSE(SkillSurrenderPlan::isSelectable(&higher, false));
	EXPECT_TRUE(SkillSurrenderPlan::isSelectable(&novice, true));
	EXPECT_TRUE(SkillSurrenderPlan::isSelectable(&higher, true));

	std::vector<Skill*> plan{&novice};
	EXPECT_FALSE(SkillSurrenderPlan::build({&novice, &higher}, &novice, plan));
	EXPECT_TRUE(plan.empty());
	EXPECT_FALSE(SkillSurrenderPlan::build({&novice, &higher}, &higher, plan, false));
	EXPECT_TRUE(plan.empty());
}

TEST(SkillSurrenderPlanTest, AdministrativePilotPermissionDoesNotBypassProtectedSkillsOrDependents) {
	SurrenderTestSkill novice("pilot_imperial_navy_novice", 0);
	SurrenderTestSkill hidden("pilot_hidden_marker", 0);
	SurrenderTestSkill godOnly("pilot_admin_marker", 0);
	SurrenderTestSkill profession("pilot_imperial_navy", 0);
	SurrenderTestSkill forceTitle("force_title_jedi_rank_03", 0);
	SurrenderTestSkill forceRank("force_rank_light_novice", 0);
	SurrenderTestSkill language("social_language_basic", 0);
	SurrenderTestSkill negative("pilot_negative_points", -1);
	hidden.makeHidden();
	godOnly.makeGodOnly();
	profession.makeProfessionRoot();

	for (auto protectedSkill : std::vector<SurrenderTestSkill*>{&hidden, &godOnly, &profession, &forceTitle, &forceRank, &language, &negative}) {
		protectedSkill->require(novice);
		EXPECT_FALSE(SkillSurrenderPlan::isSelectable(protectedSkill, true));
		std::vector<Skill*> plan{&novice};
		EXPECT_FALSE(SkillSurrenderPlan::build({&novice, protectedSkill}, protectedSkill, plan, true));
		EXPECT_TRUE(plan.empty());
		EXPECT_FALSE(SkillSurrenderPlan::build({&novice, protectedSkill}, &novice, plan, true));
		EXPECT_TRUE(plan.empty());
	}
}

} // namespace
