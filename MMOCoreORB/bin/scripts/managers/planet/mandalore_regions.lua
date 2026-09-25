-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

mandalore_regions = {
	{"@mandalore_region_names:keldabe", 1589, -6385, {CIRCLE, 500}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@mandalore_region_names:sundari", 6306, -6231, {CIRCLE, 500}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@mandalore_region_names:norg_bral", -6621, 5507, {CIRCLE, 450}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@mandalore_region_names:bralsin", -5683, -5033, {CIRCLE, 175}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@mandalore_region_names:enceri", 4709, 7155, {CIRCLE, 125}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@mandalore_region_names:shuror", 1069, 2736, {CIRCLE, 125}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
