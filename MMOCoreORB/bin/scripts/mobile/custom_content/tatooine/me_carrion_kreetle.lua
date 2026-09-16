me_carrion_kreetle = Creature:new {
	objectName = "@mob/creature_names:me_carrion_kreetle",
	customName = "",
	socialGroup = "kreetle",
	faction = "",
	mobType = MOB_CARNIVORE,
	level = 10,
	chanceHit = 0.23,
	damageMin = 55,
	damageMax = 65,
	baseXp = 65,
	baseHAM = 100,
	baseHAMmax = 140,
	armor = 0,
	resists = {0, 0, 0, 0, 0, 0, 0, -1, -1},
	meatType = "meat_insect",
	meatAmount = 5,
	hideType = "hide_scaley",
	hideAmount = 5,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0.25,
	ferocity = 3,
	pvpBitmask = ATTACKABLE,
	creatureBitmask = PACK + HERD,
	optionsBitmask = AIENABLED,
	diet = CARNIVORE,
	scale = 0.85,
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

CreatureTemplates:addCreatureTemplate(me_carrion_kreetle, "me_carrion_kreetle")
