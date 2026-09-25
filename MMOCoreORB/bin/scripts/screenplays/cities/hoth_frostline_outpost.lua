HothFrostlineOutpostScreenPlay = ScreenPlay:new {
	numberOfActs = 1,
	screenplayName = "HothFrostlineOutpostScreenPlay",
	planet = "hoth",

	-- Snapshot IDs are fixed so indoor coordinates cannot fall back to the world.
	-- {building ID, server template, world x, world y, interior cell count}
	requiredBuildings = {
		{1900100000, "object/building/military/outpost_starport.iff", 671, 2170, 1},
		{1900100200, "object/building/military/military_base_shed_imperial_style_bank_s01.iff", 747, 2035, 6},
		{1900100300, "object/building/military/military_base_shed_imperial_style_cantina_s01.iff", 671, 1950, 6},
		{1900100600, "object/building/military/outpost_shed_s02.iff", 595, 2110, 6},
	},

	-- {template, x, height, y, cell, heading in degrees}
	sceneObjects = {
		{"object/tangible/terminal/terminal_bank.iff", 0, 0.125, -3, 1900100203, 0},
		{"object/tangible/crafting/station/public_clothing_station.iff", 615, 0, 2104, 0, 90},
		{"object/tangible/crafting/station/public_food_station.iff", 615, 0, 2108, 0, 90},
		{"object/tangible/crafting/station/public_structure_station.iff", 615, 0, 2112, 0, 90},
		{"object/tangible/crafting/station/public_weapon_station.iff", 615, 0, 2116, 0, 90},
	},

	-- {template, x, height, y, heading, cell, mood, trainer}
	mobiles = {
		{"trainer_artisan", 642, 0, 2060, 90, 0, "neutral", true},
		{"trainer_brawler", 650, 0, 2020, 45, 0, "neutral", true},
		{"trainer_marksman", 664, 0, 2015, 0, 0, "neutral", true},
		{"trainer_medic", 642, 0, 2035, 90, 0, "neutral", true},
		{"trainer_scout", 664, 0, 2060, 180, 0, "neutral", true},
		{"trainer_entertainer", 686, 0, 2015, -45, 0, "neutral", true},
		{"pilot", 666, 0, 2080, 90, 0, "conversation", false},
		{"pilot", 670, 0, 2080, -90, 0, "conversation", false},
		{"commoner_technician", 620, 0, 2118, -90, 0, "neutral", false},
		{"scientist", 700, 0, 2085, -90, 0, "neutral", false},
		{"entertainer", 3, 0.125266, 2, 90, 1900100304, "themepark_music_1", false},
		{"entertainer", 5, 0.125266, 2, -90, 1900100304, "entertained", false},
	}
}

registerScreenPlay("HothFrostlineOutpostScreenPlay", true)

function HothFrostlineOutpostScreenPlay:isSnapshotReady()
	for _, entry in ipairs(self.requiredBuildings) do
		local pBuilding = getSceneObject(entry[1])

		if (pBuilding == nil or not SceneObject(pBuilding):isBuildingObject()
			or SceneObject(pBuilding):getZoneName() ~= self.planet
			or SceneObject(pBuilding):getTemplateObjectPath() ~= entry[2]
			or math.abs(SceneObject(pBuilding):getWorldPositionX() - entry[3]) > 1
			or math.abs(SceneObject(pBuilding):getWorldPositionY() - entry[4]) > 1) then
			printLuaError(self.screenplayName .. ": required Hoth snapshot building " .. entry[1] .. " is missing or invalid; skipping town spawns.")
			return false
		end

		for cellNumber = 1, entry[5] do
			local cellID = entry[1] + cellNumber
			local pCell = getSceneObject(cellID)

			if (pCell == nil or not SceneObject(pCell):isCellObject()
				or SceneObject(pCell):getZoneName() ~= self.planet
				or SceneObject(pCell):getParentID() ~= entry[1]) then
				printLuaError(self.screenplayName .. ": required Hoth snapshot cell " .. cellID .. " is missing or invalid; skipping town spawns.")
				return false
			end
		end
	end

	-- Bazaar listings retain their terminal ID, so the bazaar belongs in the
	-- snapshot instead of receiving a different screenplay object ID each boot.
	local pBazaar = getSceneObject(1900100900)
	if (pBazaar == nil or SceneObject(pBazaar):getZoneName() ~= self.planet
		or SceneObject(pBazaar):getTemplateObjectPath() ~= "object/tangible/terminal/terminal_bazaar.iff"
		or SceneObject(pBazaar):getParentID() ~= 0
		or math.abs(SceneObject(pBazaar):getWorldPositionX() - 700) > 1
		or math.abs(SceneObject(pBazaar):getWorldPositionY() - 2035) > 1) then
		printLuaError(self.screenplayName .. ": required Hoth snapshot bazaar is missing or invalid; skipping town spawns.")
		return false
	end

	return true
end

function HothFrostlineOutpostScreenPlay:start()
	if (not isZoneEnabled(self.planet) or not self:isSnapshotReady()) then
		return false
	end

	local objectsReady = self:spawnSceneObjects()
	local mobilesReady = self:spawnMobiles()
	return objectsReady and mobilesReady
end

function HothFrostlineOutpostScreenPlay:spawnSceneObjects()
	local success = true
	for _, entry in ipairs(self.sceneObjects) do
		local pObject = spawnSceneObject(self.planet, entry[1], entry[2], entry[3], entry[4], entry[5], math.rad(entry[6]))
		if (pObject == nil) then
			printLuaError(self.screenplayName .. ": unable to spawn service " .. entry[1])
			success = false
		end
	end
	return success
end

function HothFrostlineOutpostScreenPlay:spawnMobiles()
	local success = true
	for _, entry in ipairs(self.mobiles) do
		local pMobile = spawnMobile(self.planet, entry[1], 0, entry[2], entry[3], entry[4], entry[5], entry[6])
		if (pMobile == nil) then
			printLuaError(self.screenplayName .. ": unable to spawn mobile " .. entry[1])
			success = false
		else
			AiAgent(pMobile):addObjectFlag(AI_STATIC)
			CreatureObject(pMobile):setPvpStatusBitmask(0)
			CreatureObject(pMobile):setFaction(0)
			CreatureObject(pMobile):setOptionBit(INVULNERABLE)
			CreatureObject(pMobile):clearOptionBit(AIENABLED)
			CreatureObject(pMobile):setMoodString(entry[7])

			-- Keep the six trainers' normal skill conversations. Ambient workers
			-- should not offer their generic off-world mission conversations.
			if (not entry[8]) then
				CreatureObject(pMobile):clearOptionBit(CONVERSABLE)
				CreatureObject(pMobile):clearOptionBit(INTERESTING)
			end
		end
	end
	return success
end
