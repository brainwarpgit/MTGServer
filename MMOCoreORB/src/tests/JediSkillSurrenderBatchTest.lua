-- Standalone Lua regression checks; run from MMOCoreORB/src with a Lua interpreter.
-- Loads the production Jedi policies with lightweight server bindings.
package.path = "../bin/scripts/?.lua;" .. package.path
_TEST = true

ScreenPlay = {}
function ScreenPlay:new(object)
	return setmetatable(object or {}, { __index = self })
end

package.preload["screenplays.screenplay"] = function() return ScreenPlay end
package.preload["utils.logger"] = function() return {} end
package.preload["managers.quest.quest_manager"] = function() return {} end
registerScreenPlay = function() end
CreatureObject = function(player) return player end

local base = require("managers.jedi.jedi_manager")
local village = require("managers.jedi.village_jedi_manager")
local checks = 0

local function expect(actual, expected, description)
	assert(actual == expected, description)
	checks = checks + 1
end

local function player(padawan, knight, sensitiveCount, jediPoints, fullTrees)
	return {
		messages = 0,
		hasSkill = function(self, name)
			return (name == "force_title_jedi_rank_02" and padawan) or (name == "force_title_jedi_rank_03" and knight)
		end,
		getForceSensitiveSkillCount = function() return sensitiveCount end,
		villageKnightPrereqsMet = function() return jediPoints >= 206 and fullTrees >= 2 end,
		sendSystemMessage = function(self) self.messages = self.messages + 1 end,
	}
end

local sensitive = "force_sensitive_combat_prowess_ranged_accuracy_01"
local discipline = "force_discipline_light_saber_one_hand_01"
local ordinary = player(false, false, 0, 0, 0)

expect(base:canSurrenderSkillInBatch(ordinary, sensitive, 0, 0, 0), true, "Default policy allows surrender")
expect(base:new():canSurrenderSkillInBatch(ordinary, sensitive, 0, 0, 0), true, "Inherited default policy allows surrender")

local custom = base:new { canSurrenderSkill = function() return true end }
expect(custom:canSurrenderSkillInBatch(ordinary, sensitive, 0, 0, 0), false, "Custom rules require explicit projected validation")
local customVillage = village:new { canSurrenderSkill = function() return true end }
expect(customVillage:canSurrenderSkillInBatch(ordinary, sensitive, 0, 0, 0), false, "Village subclasses cannot bypass custom rules")
expect(village:canSurrenderSkillInBatch(nil, sensitive, 0, 0, 0), false, "Missing player is rejected")

-- Compare the silent projected policy with the existing single-removal policy
-- across progression state, threshold edges, and zero-point novice/master boxes.
local names = {
	sensitive,
	"force_sensitive_combat_prowess_novice",
	"force_sensitive_combat_prowess_master",
	discipline,
	"force_discipline_light_saber_one_hand_04",
	"force_title_jedi_novice",
	"force_title_jedi_rank_02",
}

for _, padawan in ipairs({ false, true }) do
	for _, knight in ipairs({ false, true }) do
		for _, count in ipairs({ 0, 24, 25, 32 }) do
			for _, points in ipairs({ 0, 205, 206, 250 }) do
				for _, trees in ipairs({ 0, 1, 2, 4 }) do
					for _, name in ipairs(names) do
						local original = player(padawan, knight, count, points, trees)
						local projected = player(padawan, knight, count, points, trees)
						local expected = village:canSurrenderSkill(original, name)
						expect(village:canSurrenderSkillInBatch(projected, name, count, points, trees), expected, "Projected rule differs for " .. name)
						expect(projected.messages, 0, "Projected rule must stay silent for " .. name)
					end
				end
			end
		end
	end
end

local padawan = player(true, false, 25, 0, 0)
expect(village:canSurrenderSkillInBatch(padawan, sensitive, 25, 0, 0), true, "First FS removal may leave 24 boxes")
expect(village:canSurrenderSkillInBatch(padawan, sensitive, 24, 0, 0), false, "Further FS removal cannot fall below 24")
expect(village:canSurrenderSkillInBatch(padawan, "force_sensitive_combat_prowess_novice", 24, 0, 0), false, "Zero-point novice is also blocked at 24")
expect(village:canSurrenderSkillInBatch(padawan, "force_sensitive_combat_prowess_master", 24, 0, 0), false, "Zero-point master is also blocked at 24")

local knight = player(true, true, 32, 220, 3)
expect(village:canSurrenderSkillInBatch(knight, discipline, 32, 212, 3), true, "First Jedi removal may preserve Knight requirements")
expect(village:canSurrenderSkillInBatch(knight, discipline, 32, 204, 3), false, "Cumulative Jedi points must preserve Knight requirements")
expect(village:canSurrenderSkillInBatch(knight, discipline, 32, 220, 2), true, "Two completed Jedi branches are enough")
expect(village:canSurrenderSkillInBatch(knight, discipline, 32, 220, 1), false, "Cumulative branch removals cannot leave only one")
expect(knight.messages + padawan.messages, 0, "Batch previews must not send denial messages")

print("Jedi skill surrender batch checks passed: " .. checks)
