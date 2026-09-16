ep3_clone_relics_wookie_prisoner = Creature:new {
	customName = "Wookiee Prisoner",
	randomNameType = NAME_GENERIC_TAG,
	socialGroup = "wookiee",
	faction = "",
	mobType = MOB_NPC,
	level = 45,
	chanceHit = 0.45,
	damageMin = 750,
	damageMax = 900,
	baseXp = 4500,
	baseHAM = 20000,
	baseHAMmax = 30000,
	armor = 2,
	resists = {60, 60, 60, 60, 60, 60, 60, 60, -1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0,
	ferocity = 0,
	pvpBitmask = NONE,
	creatureBitmask = NONE,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,
	scale = 1,
	customAiMap = "",

	templates = {
		"object/mobile/ep3/ep3_clone_relics_wookie_prisoner_01.iff",
		"object/mobile/ep3/ep3_clone_relics_wookie_prisoner_02.iff",
		"object/mobile/ep3/ep3_clone_relics_wookie_prisoner_03.iff",
		"object/mobile/ep3/ep3_clone_relics_wookie_prisoner_04.iff"
	},

	lootGroups = {
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "unarmed",
	secondaryWeapon = "none",
	conversationTemplate = ""
}

CreatureTemplates:addCreatureTemplate(ep3_clone_relics_wookie_prisoner, "ep3_clone_relics_wookie_prisoner")
