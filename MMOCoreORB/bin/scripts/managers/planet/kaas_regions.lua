-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

kaas_regions = {
	{"@kaas_region_names:dark_temple", -5117, -2291, {CIRCLE, 200}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
