-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

coruscant_regions = {
	{"@coruscant_region_names:entertainment_district", 2248, -4462, {CIRCLE, 300}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@coruscant_region_names:monument_square", 1566, 662, {CIRCLE, 300}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@coruscant_region_names:spaceport_district", -114, 3227, {CIRCLE, 300}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	{"@coruscant_region_names:coco_district", -1918, -134, {CIRCLE, 300}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
	-- Authored district label; the current snapshot has no settlement here.
	{"@coruscant_region_names:palace_district", -472, 6679, {CIRCLE, 400}, NAMEDREGION},
}
