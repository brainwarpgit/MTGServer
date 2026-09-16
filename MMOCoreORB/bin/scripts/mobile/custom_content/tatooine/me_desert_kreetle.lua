me_desert_kreetle = Creature:new {
	objectName = "@mob/creature_names:me_desert_kreetle",
	customName = "",
	socialGroup = "kreetle",
	faction = "",
	mobType = MOB_CARNIVORE,
	level = 7,
	chanceHit = 0.23,
	damageMin = 55,
	damageMax = 75,
	baseXp = 85,
	baseHAM = 100,
	baseHAMmax = 160,
	armor = 0,
	resists = {0, 0, 0, 0, 0, 0, 0, -1, -1},
	meatType = "meat_insect",
	meatAmount = 6,
	hideType = "hide_scaley",
	hideAmount = 6,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0.25,
	ferocity = 3,
	pvpBitmask = ATTACKABLE,
	creatureBitmask = HERD,
	optionsBitmask = AIENABLED,
	diet = CARNIVORE,
	scale = 0.95,
	customAiMap = "",

	templates = {
		"object/mobile/kreetle.iff"
	},
	controlDeviceTemplate = "object/intangible/pet/bark_mite_hue.iff",

	lootGroups = {
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "unarmed",
	secondaryWeapon = "none",

	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = {
		{"intimidationattack", ""}
	},
	secondaryAttacks = {},
	conversationTemplate = ""
}

CreatureTemplates:addCreatureTemplate(me_desert_kreetle, "me_desert_kreetle")
