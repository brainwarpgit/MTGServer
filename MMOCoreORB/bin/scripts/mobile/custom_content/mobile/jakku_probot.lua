jakku_probot = Creature:new {
	objectName = "@mob/creature_names:imperial_probot_drone",
	customName = "a Protocol-77 Probot",
	socialGroup = "death_watch",
	faction = "",
	mobType = MOB_NPC,
	level = 140,
	chanceHit = 0.31,
	damageMin = 1500,
	damageMax = 1600,
	baseXp = 710,
	baseHAM = 1440,
	baseHAMmax = 1680,
	armor = 0,
	resists = {0, 0, 0, 0, 0, 0, 0, -1, -1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0,
	ferocity = 0,
	pvpBitmask = AGGRESSIVE + ATTACKABLE + ENEMY,
	creatureBitmask = PACK,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,
	scale = 1,
	customAiMap = "",

	templates = {
		"object/mobile/probot.iff"
	},

	lootGroups = {
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "droid_probot_ranged",
	secondaryWeapon = "none",
	conversationTemplate = "",
	defaultAttack = "attack"
}

CreatureTemplates:addCreatureTemplate(jakku_probot, "jakku_probot")
