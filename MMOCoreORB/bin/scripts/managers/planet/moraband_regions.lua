-- City circles match the client region table for names and enter/leave boundaries.
-- Covers the existing military outpost without including the nearby temple ruins.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

moraband_regions = {
	{"@moraband_region_names:moraband_outpost", -1715, -641, {CIRCLE, 125}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
