--Copyright (C) 2007 <SWGEmu>

--This File is part of Core3.

--This program is free software; you can redistribute
--it and/or modify it under the terms of the GNU Lesser
--General Public License as published by the Free Software
--Foundation; either version 2 of the License,
--or (at your option) any later version.

--This program is distributed in the hope that it will be useful,
--but WITHOUT ANY WARRANTY; without even the implied warranty of
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
--See the GNU Lesser General Public License for
--more details.

--You should have received a copy of the GNU Lesser General
--Public License along with this program; if not, write to
--the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

--Linking Engine3 statically or dynamically with other modules
--is making a combined work based on Engine3.
--Thus, the terms and conditions of the GNU Lesser General Public License
--cover the whole combination.

--In addition, as a special exception, the copyright holders of Engine3
--give you permission to combine Engine3 program with free software
--programs or libraries that are released under the GNU LGPL and with
--code included in the standard release of Core3 under the GNU LGPL
--license (or modified versions of such code, with unchanged license).
--You may copy and distribute such a system following the terms of the
--GNU LGPL for Engine3 and the licenses of the other code concerned,
--provided that you include the source code of that other code when
--and as the GNU LGPL requires distribution of source code.

--Note that people who make modified versions of Engine3 are not obligated
--to grant this special exception for their modified versions;
--it is their choice whether to do so. The GNU Lesser General Public License
--gives permission to release a modified version without this exception;
--this exception also makes it possible to release a modified version
--which carries forward this exception.

-- Core3 server configuration
-- Existing values are preserved; newly listed settings use their source defaults.
-- Boolean settings accept true/false or 1/0. Durations include their units below.
-- Commented overrides preserve defaults that depend on other settings or the build.
-- Engine properties use the separate core3engine configuration, not this table.
-- MOTD and Revision are loaded from conf/motd.txt and conf/rev.txt.

Core3 = {

	-- Services and network endpoints
	-- ------------------------------
	MakeLogin = 1,
	MakeZone = 1,
	MakePing = 1,
	MakeStatus = 1,
	ORB = "",
	ORBPort = 44419,
	LoginPort = 44453,
	LoginAllowedConnections = 3000,
	PingPort = 44462,
	PingAllowedConnections = 3000,
	StatusPort = 44455,
	StatusAllowedConnections = 500,
	StatusInterval = 30, -- Seconds to cache zone status.
	ZoneGalaxyID = 2, -- Must match the galaxy entry used by the login server.
	ZoneServerPort = 0, -- 0 uses the port from the galaxy database/API entry.
	ZoneAllowedConnections = 30000,
	ZonePortsBalancer = 1, -- USE_RANDOM_EXTRA_PORTS builds: 1 = round robin; other values = random.
	Zone = {
		ThreadsDefault = 1, -- Worker threads per ground zone.
		-- ThreadsCorellia = 1, -- Example: Threads + capitalized zone name; otherwise ThreadsDefault.
	},
	SpaceZone = {
		ThreadsDefault = 1, -- Worker threads per space zone.
		-- ThreadsSpaceCorellia = 1, -- Example: ThreadsSpace + capitalized planet name.
	},

	-- Database connections
	-- --------------------
	DBHost = "127.0.0.1",
	DBPort = 3306,
	DBName = "swgemu",
	DBUser = "swgemu",
	DBPass = "123456",
	DBInstances = 2,
	DBSecret = "swgemus3cr37!", -- Use a unique server secret; override credentials in config-local.lua.
	-- Mantis is used by the optional in-game support-ticket integration.
	MantisHost = "127.0.0.1",
	MantisPort = 3306,
	MantisName = "swgemu",
	MantisUser = "swgemu",
	MantisPass = "123456",
	MantisPrfx = "mantis_", -- Mantis table prefix.

	-- Accounts, login and terms of service
	-- ------------------------------------
	AutoReg = 1,
	RegistrationMessage = "Automatic registration is currently disabled. Please contact the administrators of the server in order to get an authorized account.",
	LoginRequiredVersion = "20050408-18:00",
	InactiveAccountTitle = "Account Disabled",
	InactiveAccountText = "The server administrators have disabled your account.",
	TermsOfServiceVersion = 0,
	TermsOfService = "",
	AccountManager = {
		CreatedDateFirstConnect = false, -- Record account creation time on first connection.
		HolocronTicketsEnabled = false, -- Enable in-game Mantis support tickets.
	},
	Login = {
		EnableSessionId = false, -- Enable session-ID authentication.
		SessionDuration = "00:15", -- Session lifetime in HH:MM.
		-- External sessions API; requires an WITH_SWGREALMS_API build and URL/token.
		API = {
			BaseURL = "",
			APIToken = "",
			DryRun = false,
			FailOpen = false,
			WorkerThreads = 4,
			DebugLevel = 0,
			Timeout = 30, -- HTTP timeout in seconds.
			MetricsInterval = 600, -- Seconds between API metric updates.
			-- RotateLogSizeMB = 100, -- Omit to inherit Core3.RotateLogSizeMB.
			-- StreamURL = "", -- Omit to derive the ws/wss URL from BaseURL and galaxy ID.
		},
	},

	-- Characters and player behavior
	-- ------------------------------
	CharacterBuilderEnabled = "true",
	PurgeDeletedCharacters = 10, -- Minutes between deleted-character cleanup passes.
	CleanupMailCount = 25000, -- Maximum expired mails deleted during startup.
	PlayerCreationManager = {
		MaxCharactersPerGalaxy = 10,
		EnableTutorial = false,
	},
	PlayerManager = {
		ValidClientVersion = "20050408-18:00", -- Zone handshake version; separate from LoginRequiredVersion.
		GalaxyWideGrouping = false,
		DisableGroupVisibility = false,
		AdvancedWaypoints = false,
		WipeFillingOnClone = false,
		accountVictimList = false, -- Track PvP victims per account instead of per character.
	},
	PlayerObject = {
		AlwaysSafeLogout = false,
		LinkDeadDelay = 3 * 60, -- Seconds before unsafe link-dead logout.
	},
	SameAccountTipsAreFree = false,
	NameManager = {
		FilterTable = "oldFilterWords", -- Name-filter table in the name-manager Lua data.
	},

	-- Enabled ground and space zones
	-- ------------------------------
	ZonesEnabled = {
		-------- PRE-CU ZONES -------
		"corellia",
		"dantooine",
		"dathomir",
		"dungeon1",
		"endor",
		"lok",
		"naboo",
		"rori",
		"talus",
		"tatooine",
		"tutorial",
		"yavin4",
		-------- NEW CONTENT ZONES -------
		"chandrila",
		"coruscant",
		"dungeon2",
		"hoth",
		"kaas",
		"kashyyyk",
		"kashyyyk_hunting",
		"kashyyyk_rryatt_trail",
		"kashyyyk_main",
		"kashyyyk_south_dungeons",
		"kashyyyk_north_dungeons",
		"kashyyyk_pob_dungeons",
		"kashyyyk_dead_forest",
		"mandalore",
		"moraband",
		"mustafar",
		"taanab",
		-------- TEST ZONES -------
		--"09",
		--"10",
		--"11",
		--"character_farm",
		--"cinco_city_test_m5",
		--"creature_test",
		--"endor_asommers",
		--"floratest",
		--"godclient_test",
		--"otoh_gunga",
		--"rivertest",
		--"runtimerules",
		--"simple",
		--"taanab",
		--"test_wearables",
		--"umbra",
		--"watertabletest",
	},
	SpaceZonesEnabled = {
		"space_corellia",
		"space_dantooine",
		"space_dathomir",
		"space_endor",
		"space_heavy1",
		"space_light1",
		"space_lok",
		"space_naboo",
		"space_tatooine",
		"space_yavin4",
		---- TEST ZONES ----
		--"space_09",
		--"space_corellia_2",
		--"space_env",
		--"space_halos",
		--"space_naboo_2",
		--"space_tatooine_2",
	},

	-- Client assets and TRE archives
	-- ------------------------------
	TrePath = "/home/swgemu/workspace/tre",
	-- Keep archive precedence/order intact when adding patches.
	TreFiles = {
		"mtg_patch_024.tre",
		"mtg_patch_023.tre",
		"mtg_patch_022.tre",
		"mtg_planets.tre",
		"mtg_patch_021.tre",
		--"mtg_patch_020.tre",
		"mtg_patch_019.tre",
		"mtg_patch_018.tre",
		"mtg_patch_017.tre",
		"mtg_patch_016.tre",
		"mtg_patch_015.tre",
		"mtg_patch_014.tre",
		"mtg_patch_013_configurable_02.tre",
		"mtg_patch_012_configurable_01.tre",
		"mtg_patch_011_files_01.tre",
		"mtg_patch_010_object_01.tre",
		"mtg_patch_009_Shader_01.tre",
		"mtg_patch_008_texture_04.tre",
		"mtg_patch_007_texture_03.tre",
		"mtg_patch_006_texture_02.tre",
		"mtg_patch_005_texture_01.tre",
		"mtg_patch_004_appearance_04.tre",
		"mtg_patch_003_appearance_03.tre",
		"mtg_patch_002_appearance_02.tre",
		"mtg_patch_001_appearance_01.tre",
	},
	TreManager = {
		ReloadStrings = false, -- Reload requested string tables for development.
	},

	-- World spawning, AI and navigation
	-- ---------------------------------
	MustafarMapBounds = true, -- Keep all players, including admins, within Mustafar's mapped area.
	Regions = {
		DisableWorldSpawns = false,
		DisableSpaceSpawns = false,
		minimumLairSpawnInterval = 5000, -- Milliseconds between ground spawn attempts.
		minimumSpaceSpawnInterval = 5000, -- Milliseconds between space spawn attempts.
		spawnCheckRange = 64, -- Meters.
		spaceSpawnCheckRange = 1024, -- Meters.
	},
	AiAgent = {
		AiAgentLoadTesting = false, -- DEBUG_AI builds only: keep AI active for load testing.
		Verbose = false, -- Verbose behavior-tree logging.
		-- LogLevel = 2, -- Optional override; startup defaults to WARNING, later paths use -1.
		-- ConsoleThrottle = 100, -- Default is 100, or 1 in DEBUG_AI builds; must be positive.
		-- Per-template example: stormtrooper = { LogLevel = 5 },
	},
	ShipAiAgent = {
		LogLevel = 2, -- Initial ship AI logger level.
	},
	MaxNavMeshJobs = 6,
	DumpObjFiles = 1, -- Export navigation geometry OBJ files.
	NavMeshManager = {
		LogLevel = 4,
	},
	UnloadContainers = 1, -- Unload inactive container contents from RAM.

	-- Combat, faction warfare and space features
	-- ------------------------------------------
	PvpMode = false,
	CombatManager = {
		AllowSameAccountLinkDeadBeneficialActions = true,
	},
	GCWManager = {
		useCovertOvertSystem = false,
	},
	ChatManager = {
		PvpBroadcastChannel = false,
	},
	JTL = {
		JTLEnabled = false, -- Enable JTL features exposed to screenplays.
	},
	FrsManager = {
		ImmediateMaintXpDeduction = false,
	},

	-- Missions and bounties
	-- ---------------------
	MissionManager = {
		IncludeFactionPets = true, -- Include faction pets in mission difficulty.
		ListRequestCooldown = 1400, -- Milliseconds between mission-list requests.
		AnonymousBountyTerminals = false,
		MaxBountiesPerJedi = 5,
		PrivateStructureJediMissions = true,
		BountyExpirationTime = 172800000, -- Milliseconds; 48 hours.
		PlayerBountyCooldownTime = 86400000, -- Milliseconds; 24 hours.
		-- PlayerBountyCooldown = true, -- Omit to retain existing caller-specific true/false defaults.
	},

	-- Structures, maintenance and travel
	-- ----------------------------------
	StructureManager = {
		AnyPlanet = false, -- Ignore structure planet lists; other placement rules still apply.
		RequireDestroyCode = true, -- Require the numeric demolition code after the Yes/No confirmation.
		EnhancedFurnitureRotate = false,
		-- Optional per-client-structure navmesh flags, keyed by exact objectName.getFullPath():
		-- CreateNavMesh = { ["<exact objectName.getFullPath()>"] = false },
	},
	StructureMaintenanceTask = {
		AllowBankPayments = true,
	},
	StructureObject = {
		MaintenanceBootDelay = 600, -- Seconds, plus a random delay of up to one hour.
	},
	Tweaks = {
		StructureObject = {
			DestroyOrphans = false, -- Remove orphaned civic structures.
		},
	},
	ShuttleZoneComponent = {
		BootDelay = 5 * 60 * 1000, -- Milliseconds before initial shuttle scheduling.
	},
	-- DEBUG_TRAVEL builds only: uncomment to override per-planet Lua travel timings.
	-- PlanetManager = {
		-- ShuttleportAwayTime = 300, -- Seconds; sample fallback value.
		-- ShuttleportLandedTime = 120, -- Seconds; sample fallback value.
		-- ShuttleportLandingTime = 11, -- Seconds; sample fallback value.
		-- StarportAwayTime = 60, -- Seconds; sample fallback value.
		-- StarportLandedTime = 120, -- Seconds; sample fallback value.
		-- StarportLandingTime = 14, -- Seconds; sample fallback value.
	-- },

	-- Items, loot and auctions
	-- ------------------------
	TangibleObject = {
		ShowTemplate = false, -- Show server/shared .iff template paths in Examine for all viewers.
		NoTradeMessage = "", -- Text appended to the item condition attribute.
		ForceNoTradeMessage = "", -- Text appended to the item condition attribute.
		ForceNoTradeADKMessage = "", -- Text appended to the item condition attribute.
	},
	LootManager = {
		DebugAttributes = false,
	},
	MaxAuctionSearchJobs = 1,
	AuctionManager = {
		LogLevel = -1,
		-- RotateLogSizeMB = 100, -- Omit to inherit Core3.RotateLogSizeMB.
		Startup = {
			ExpireInvalid = false,
		},
	},
	AuctionItem = {
		ExportOnDestroy = false,
	},

	-- Logging and metrics
	-- -------------------
	-- Logger levels: -1 NONE, 0 FATAL, 1 ERROR, 2 WARNING, 3 LOG, 4 INFO, 5 DEBUG.
	LogFile = "log/core3.log",
	LogFileLevel = 4,
	LogJSON = 0,
	LogSync = 0,
	LuaLogJSON = 0,
	PathfinderLogJSON = 0,
	PlayerLogLevel = 4,
	RotateLogSizeMB = 100, -- Default rotation size for loggers that use size-based rotation.
	RotateLogAtStart = false,
	MaxLogLines = 1000000, -- Lines before rotating player.log.
	ProgressMonitors = "true",
	SessionStatsSeconds = 1800, -- Seconds between periodic statistics writes; clamped to 300-3600 outside WITH_DEV_MODE.
	OnlineLogSeconds = 300, -- Seconds between online-player log updates.
	OnlineLogSize = 100000000, -- Bytes before rotating the online-player log.
	LogOnlineCount = 3, -- Accounts sharing one IP before detailed logging starts.
	LogOnlineOnSessionChange = true,
	ZoneServer = {
		ClientLogLevel = -1, -- Per-client logging; supports account-specific overrides below.
	},
	UseMetrics = false,
	MetricsHost = "localhost",
	MetricsPort = 8125,
	MetricsPrefix = "",

	-- Lua screenplays and command diagnostics
	-- ---------------------------------------
	DirectorManager = {
		SlowLoadMs = 1000, -- Milliseconds before reporting a slow screenplay load.
	},
	LuaEngine = {
		LogLevel = 1,
		LuaEventLogLevel = 4,
	},
	CommandConfigManager = {
		DumpAdminCommands = false,
	},
	-- Optional per-command cooldown overrides, in milliseconds; omitted commands use their own defaults.
	-- CommandCooldown = { attack = 1000 },

	-- REST API and object exports
	-- ---------------------------
	RESTServerPort = 0, -- 0 disables the REST server; requires an WITH_REST_API build.
	RESTServer = {
		APIToken = "",
		SSLKeyFile = "",
		SSLCertFile = "",
		WorkerThreads = 4,
		LogLevel = 4,
		-- RotateLogSizeMB = 100, -- Omit to inherit Core3.RotateLogSizeMB.
		exportDir = "log/exports/api/%Y-%m-%d/%H/", -- strftime-style export directory.
	},
	SceneObject = {
		exportDir = "log/exports/%Y-%m-%d/%H/", -- strftime-style export directory.
	},

	-- Transaction logging
	-- -------------------
	TransactionLog = {
		Enabled = false,
		AsyncExport = false,
		Verbose = false,
		WorkerThreads = 4,
		LogLevel = 5,
		PruneCreatureObjects = true,
		PruneCraftedComponents = true,
		CheckPlayerDebug = true,
		-- RotateLogSizeMB = 100, -- Omit to inherit Core3.RotateLogSizeMB.
	},

	-- Account-specific overrides
	-- --------------------------
	-- Account IDs must be strings. The full Core3 key is required inside each account table.
	-- AccountFlags = {
	-- 	["12345"] = { ["Core3.ZoneServer.ClientLogLevel"] = 5 },
	-- },
}

-- conf/config-local.lua is loaded afterward. Use individual Core3.Key assignments
-- there to override these defaults without replacing the entire Core3 table.
