-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

chandrila_regions = {
	{"@chandrila_region_names:dearic", 294, -2938, {CIRCLE, 800}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@chandrila_region_names:nashal", -5255, 292, {CIRCLE, 400}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
