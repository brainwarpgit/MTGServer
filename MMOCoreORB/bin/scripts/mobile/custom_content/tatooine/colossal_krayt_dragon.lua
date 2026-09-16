colossal_krayt_dragon = Creature:new {
	customName = "the Scourge of Tatooine (a colossal krayt dragon)",
	socialGroup = "krayt",
	faction = "",
	mobType = MOB_CARNIVORE,
	level = 360,
	chanceHit = 95.5,
	damageMin = 6275,
	damageMax = 7975,
	baseXp = 32549,
	baseHAM = 1350000,
	baseHAMmax = 1550000,
	armor = 3,
	resists = {195, 195, 195, 195, 195, 195, 195, 195, -1},
	meatType = "meat_carnivore",
	meatAmount = 1000,
	hideType = "hide_bristley",
	hideAmount = 950,
	boneType = "bone_mammal",
	boneAmount = 905,
	milk = 0,
	tamingChance = 0,
	ferocity = 30,
	pvpBitmask = AGGRESSIVE + ATTACKABLE + ENEMY,
	creatureBitmask = PACK + KILLER + STALKER,
	optionsBitmask = AIENABLED,
	diet = CARNIVORE,
	scale = 4,
	customAiMap = "",

	templates = {
		"object/mobile/krayt_dragon.iff"
	},

	lootGroups = {
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "creature_spit_heavy_flame",
	secondaryWeapon = "none",

	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = {},
	secondaryAttacks = {},
	conversationTemplate = ""
}

CreatureTemplates:addCreatureTemplate(colossal_krayt_dragon, "colossal_krayt_dragon")
