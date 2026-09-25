-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

taanab_regions = {
	{"@taanab_region_names:pandath", 2084, 5395, {CIRCLE, 500}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@taanab_region_names:starhunterstation", 3673, -5425, {CIRCLE, 250}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
