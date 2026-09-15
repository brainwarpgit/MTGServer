#include "gtest/gtest.h"

#include "conf/ConfigManager.h"
#include "templates/customization/CustomizationIdManager.h"
#include "templates/manager/DataArchiveStore.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace {

const char* const HUMAN_MALE = "object/creature/player/human_male.iff";
const char* const CHISS_MALE = "object/creature/player/chiss_male.iff";
const char* const SHARED_HUMAN_MALE = "object/creature/player/shared_human_male.iff";
const char* const SHARED_CHISS_MALE = "object/creature/player/shared_chiss_male.iff";
const char* const HAIR_STYLE_11 = "object/tangible/hair/human/hair_human_male_s11.iff";
const char* const HAIR_STYLE_12 = "object/tangible/hair/human/hair_human_male_s12.iff";

using Bytes = std::string;

Bytes little32(uint32_t value) {
	Bytes bytes;
	for (unsigned shift = 0; shift < 32; shift += 8)
		bytes.push_back(static_cast<char>(value >> shift));
	return bytes;
}

Bytes terminated(const char* value) {
	return Bytes(value) + '\0';
}

Bytes chunk(const char* tag, const Bytes& payload) {
	Bytes bytes(tag, 4);
	for (int shift = 24; shift >= 0; shift -= 8)
		bytes.push_back(static_cast<char>(payload.size() >> shift));
	return bytes + payload;
}

Bytes form(const char* tag, const Bytes& contents) {
	return chunk("FORM", Bytes(tag, 4) + contents);
}

struct HairRow {
	const char* sharedHair;
	const char* sharedPlayer;
	int skill;
	bool availableAtCreation;
	const char* serverHair;
	const char* serverPlayer;
};

const HairRow HUMAN_STYLE_11 = {
	"object/tangible/hair/human/shared_hair_human_male_s11.iff",
	SHARED_HUMAN_MALE, 0, true, HAIR_STYLE_11, HUMAN_MALE
};

const HairRow CHISS_STYLE_11 = {
	"object/tangible/hair/human/shared_hair_human_male_s11.iff",
	SHARED_CHISS_MALE, 60, false, HAIR_STYLE_11, CHISS_MALE
};

// Build the six-column DTII file independently of the production loader.
// IFF lengths are big endian; table integers are little endian and strings
// are null terminated. These fixtures do not require game assets or a DB.
Bytes hairTable(const std::vector<HairRow>& rows) {
	Bytes columns = little32(6);
	for (const char* name : {"sharedTemplate", "playerTemplate", "skillModValue",
			"availableAtCreation", "serverTemplate", "serverPlayerTemplate"})
		columns += terminated(name);

	Bytes types;
	for (const char* type : {"s", "s", "i", "b", "s", "s"})
		types += terminated(type);

	Bytes data = little32(rows.size());
	for (const auto& row : rows) {
		data += terminated(row.sharedHair) + terminated(row.sharedPlayer) +
			little32(row.skill) + little32(row.availableAtCreation ? 1 : 0) +
			terminated(row.serverHair) + terminated(row.serverPlayer);
	}

	return form("DTII", form("0001", chunk("COLS", columns) +
		chunk("TYPE", types) + chunk("ROWS", data)));
}

void loadRows(CustomizationIdManager& manager, const std::vector<HairRow>& rows) {
	Bytes bytes = hairTable(rows);
	IffStream stream;
	ASSERT_TRUE(stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "hair-assets-test.iff"));
	ASSERT_NO_THROW(manager.loadHairAssetsSkillMods(&stream));
}

void expectRow(const HairAssetData* hair, const HairRow& expected) {
	ASSERT_NE(nullptr, hair);
	EXPECT_EQ(String(expected.serverHair), hair->getServerTemplate());
	EXPECT_EQ(String(expected.serverPlayer), hair->getServerPlayerTemplate());
	EXPECT_EQ(String(expected.sharedHair), hair->getSharedTemplate());
	EXPECT_EQ(String(expected.sharedPlayer), hair->getPlayerTemplate());
	EXPECT_EQ(expected.skill, hair->getSkillModValue());
	EXPECT_EQ(expected.availableAtCreation, hair->isAvailableAtCreation());
}

void expectSharedStyleRows(const std::vector<HairRow>& rows) {
	CustomizationIdManager manager;
	loadRows(manager, rows);
	const HairAssetData* human = manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE);
	const HairAssetData* chiss = manager.getHairAssetData(HAIR_STYLE_11, CHISS_MALE);

	expectRow(human, HUMAN_STYLE_11);
	expectRow(chiss, CHISS_STYLE_11);
	EXPECT_NE(human, chiss);
}

} // namespace

TEST(HairAssetDataTest, SharedHairPreservesEachPlayersDataWhenHumanRowComesFirst) {
	expectSharedStyleRows({HUMAN_STYLE_11, CHISS_STYLE_11});
}

TEST(HairAssetDataTest, SharedHairPreservesEachPlayersDataWhenChissRowComesFirst) {
	expectSharedStyleRows({CHISS_STYLE_11, HUMAN_STYLE_11});
}

TEST(HairAssetDataTest, RejectsPlayersWithoutACompatibleRow) {
	CustomizationIdManager manager;
	loadRows(manager, {HUMAN_STYLE_11});

	expectRow(manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE), HUMAN_STYLE_11);
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, CHISS_MALE));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, "object/creature/player/human_female.iff"));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, "object/creature/player/wookiee_male.iff"));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, "object/creature/player/unknown_male.iff"));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, ""));
}

TEST(HairAssetDataTest, RejectsUnknownHairWithoutFallingBackToAnotherStyle) {
	CustomizationIdManager manager;
	loadRows(manager, {HUMAN_STYLE_11, CHISS_STYLE_11});

	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_12, HUMAN_MALE));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_12, CHISS_MALE));
	EXPECT_EQ(nullptr, manager.getHairAssetData("object/tangible/hair/human/unknown.iff", HUMAN_MALE));
	EXPECT_EQ(nullptr, manager.getHairAssetData("", HUMAN_MALE));
}

TEST(HairAssetDataTest, SelectsByBothHairStyleAndPlayerTemplate) {
	const HairRow humanStyle12 = {
		"object/tangible/hair/human/shared_hair_human_male_s12.iff",
		SHARED_HUMAN_MALE, 75, false, HAIR_STYLE_12, HUMAN_MALE
	};
	const HairRow chissStyle12 = {
		"object/tangible/hair/human/shared_hair_human_male_s12.iff",
		SHARED_CHISS_MALE, 25, true, HAIR_STYLE_12, CHISS_MALE
	};
	CustomizationIdManager manager;
	loadRows(manager, {HUMAN_STYLE_11, chissStyle12, CHISS_STYLE_11, humanStyle12});

	expectRow(manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE), HUMAN_STYLE_11);
	expectRow(manager.getHairAssetData(HAIR_STYLE_11, CHISS_MALE), CHISS_STYLE_11);
	expectRow(manager.getHairAssetData(HAIR_STYLE_12, HUMAN_MALE), humanStyle12);
	expectRow(manager.getHairAssetData(HAIR_STYLE_12, CHISS_MALE), chissStyle12);
}

TEST(HairAssetDataTest, DuplicatePlayerAndHairUsesLastRowsMetadata) {
	HairRow replacement = HUMAN_STYLE_11;
	replacement.skill = 90;
	replacement.availableAtCreation = false;
	CustomizationIdManager manager;
	loadRows(manager, {HUMAN_STYLE_11, CHISS_STYLE_11, replacement});

	expectRow(manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE), replacement);
	expectRow(manager.getHairAssetData(HAIR_STYLE_11, CHISS_MALE), CHISS_STYLE_11);
}

TEST(HairAssetDataTest, LookupRequiresServerTemplates) {
	CustomizationIdManager manager;
	loadRows(manager, {HUMAN_STYLE_11, CHISS_STYLE_11});

	EXPECT_EQ(nullptr, manager.getHairAssetData(HAIR_STYLE_11, SHARED_HUMAN_MALE));
	EXPECT_EQ(nullptr, manager.getHairAssetData(HUMAN_STYLE_11.sharedHair, HUMAN_MALE));
	expectRow(manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE), HUMAN_STYLE_11);
}

TEST(HairAssetIntegrationTest, ConfiguredHumanAndChissCanUseSharedMaleStyle11) {
	ConfigManager::instance()->loadConfigData();
	auto archive = DataArchiveStore::instance();
	ASSERT_EQ(0, archive->loadTres(ConfigManager::instance()->getTrePath(), ConfigManager::instance()->getTreFiles()));
	std::unique_ptr<IffStream> stream(archive->openIffFile("datatables/customization/hair_assets_skill_mods.iff"));
	ASSERT_NE(nullptr, stream);
	CustomizationIdManager manager;
	ASSERT_NO_THROW(manager.loadHairAssetsSkillMods(stream.get()));

	const HairAssetData* human = manager.getHairAssetData(HAIR_STYLE_11, HUMAN_MALE);
	const HairAssetData* chiss = manager.getHairAssetData(HAIR_STYLE_11, CHISS_MALE);
	ASSERT_NE(nullptr, human);
	ASSERT_NE(nullptr, chiss);
	EXPECT_NE(human, chiss);
	EXPECT_EQ(String(HAIR_STYLE_11), human->getServerTemplate());
	EXPECT_EQ(String(HAIR_STYLE_11), chiss->getServerTemplate());
	EXPECT_EQ(String(HUMAN_MALE), human->getServerPlayerTemplate());
	EXPECT_EQ(String(CHISS_MALE), chiss->getServerPlayerTemplate());
	EXPECT_EQ(String(SHARED_HUMAN_MALE), human->getPlayerTemplate());
	EXPECT_EQ(String(SHARED_CHISS_MALE), chiss->getPlayerTemplate());
	EXPECT_TRUE(human->isAvailableAtCreation());
	EXPECT_TRUE(chiss->isAvailableAtCreation());
}
