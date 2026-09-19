#include "gtest/gtest.h"

#include "server/zone/managers/skill/SkillSurrenderDisplay.h"

#include <vector>

namespace {

TEST(SkillSurrenderDisplayTest, SortsByDisplayedSkillNameInsteadOfInternalProfessionPrefix) {
	std::vector<SkillSurrenderDisplay::Entry> entries{
		{"social_entertainer_novice", "Novice Entertainer"},
		{"social_dancer_novice", "Novice Dancer"},
		{"social_entertainer_dance_04", "Dancing IV"},
		{"social_entertainer_master", "Master Entertainer"}
	};

	SkillSurrenderDisplay::sort(entries);

	ASSERT_EQ(4u, entries.size());
	EXPECT_TRUE(entries[0].skillName == "social_entertainer_dance_04");
	EXPECT_TRUE(entries[1].skillName == "social_entertainer_master");
	EXPECT_TRUE(entries[2].skillName == "social_dancer_novice");
	EXPECT_TRUE(entries[3].skillName == "social_entertainer_novice");
}

TEST(SkillSurrenderDisplayTest, IgnoresCaseWithoutChangingDisplayedCapitalization) {
	std::vector<SkillSurrenderDisplay::Entry> entries{
		{"skill_novice", "NOVICE ENTERTAINER"},
		{"skill_master", "Master Entertainer"},
		{"skill_dance", "dancing IV"}
	};

	SkillSurrenderDisplay::sort(entries);

	EXPECT_TRUE(entries[0].skillName == "skill_dance");
	EXPECT_TRUE(entries[0].displayName == "dancing IV");
	EXPECT_TRUE(entries[1].skillName == "skill_master");
	EXPECT_TRUE(entries[1].displayName == "Master Entertainer");
	EXPECT_TRUE(entries[2].skillName == "skill_novice");
	EXPECT_TRUE(entries[2].displayName == "NOVICE ENTERTAINER");
}

TEST(SkillSurrenderDisplayTest, UsesInternalSkillNameToBreakMatchingLabelTies) {
	std::vector<SkillSurrenderDisplay::Entry> first{
		{"skill_z", "Dancing IV"},
		{"skill_a", "Dancing IV"},
		{"skill_m", "dAnCiNg iV"}
	};
	std::vector<SkillSurrenderDisplay::Entry> second{first[2], first[1], first[0]};

	SkillSurrenderDisplay::sort(first);
	SkillSurrenderDisplay::sort(second);

	EXPECT_TRUE(first[0].skillName == "skill_a");
	EXPECT_TRUE(first[1].skillName == "skill_m");
	EXPECT_TRUE(first[2].skillName == "skill_z");
	for (std::size_t i = 0; i < first.size(); ++i)
		EXPECT_TRUE(first[i].skillName == second[i].skillName);
}

TEST(SkillSurrenderDisplayTest, SortingDisplayEntriesPreservesDependencyRemovalOrder) {
	const std::vector<String> removalOrder{
		"social_dancer_master",
		"social_dancer_novice",
		"social_entertainer_dance_04",
		"social_entertainer_novice"
	};
	std::vector<SkillSurrenderDisplay::Entry> entries{
		{removalOrder[0], "Master Dancer"},
		{removalOrder[1], "Novice Dancer"},
		{removalOrder[2], "Dancing IV"},
		{removalOrder[3], "Novice Entertainer"}
	};

	SkillSurrenderDisplay::sort(entries);

	EXPECT_TRUE(entries[0].skillName == "social_entertainer_dance_04");
	EXPECT_TRUE(entries[1].skillName == "social_dancer_master");
	EXPECT_TRUE(entries[2].skillName == "social_dancer_novice");
	EXPECT_TRUE(entries[3].skillName == "social_entertainer_novice");
	EXPECT_TRUE(removalOrder[0] == "social_dancer_master");
	EXPECT_TRUE(removalOrder[1] == "social_dancer_novice");
	EXPECT_TRUE(removalOrder[2] == "social_entertainer_dance_04");
	EXPECT_TRUE(removalOrder[3] == "social_entertainer_novice");
}

TEST(SkillSurrenderDisplayTest, AcceptsEmptyAndSingleEntryLists) {
	std::vector<SkillSurrenderDisplay::Entry> entries;
	SkillSurrenderDisplay::sort(entries);
	EXPECT_TRUE(entries.empty());

	entries.push_back({"social_entertainer_novice", "Novice Entertainer"});
	SkillSurrenderDisplay::sort(entries);
	ASSERT_EQ(1u, entries.size());
	EXPECT_TRUE(entries[0].skillName == "social_entertainer_novice");
	EXPECT_TRUE(entries[0].displayName == "Novice Entertainer");
}

} // namespace
