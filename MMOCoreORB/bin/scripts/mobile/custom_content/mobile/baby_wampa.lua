baby_wampa = Creature:new {
	customName = "a baby Wampa",
	socialGroup = "wampa",
	faction = "",
	mobType = MOB_NPC,
	level = 25,
	chanceHit = 0.6,
	damageMin = 400,
	damageMax = 700,
	baseXp = 5,
	baseHAM = 86000,
	baseHAMmax = 87000,
	armor = 0,
	resists = {0, 0, 0, 0, 0, 0, 0, 0, -1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 1,
	ferocity = 0,
	pvpBitmask = BABY,
	creatureBitmask = NONE,
	optionsBitmask = 128,
	diet = HERBIVORE,
	scale = 0.2,
	customAiMap = "",

	templates = {
		"object/mobile/wampa.iff"
	},

	lootGroups = {
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "unarmed",
	secondaryWeapon = "none",

	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = {
		{"stunattack", "stunChance=100"},
		{"creatureareaknockdown", "knockdownChance=100"}
	},
	secondaryAttacks = {},
	conversationTemplate = ""
}

CreatureTemplates:addCreatureTemplate(baby_wampa, "baby_wampa")
