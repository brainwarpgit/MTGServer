-- City circles match the client region table for names and enter/leave boundaries.
-- This covers the snapshot starport compound, not the entire Kachirho settlement.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

kashyyyk_main_regions = {
	{"@kashyyyk_main_region_names:kachirho_starport", -678, -160, {CIRCLE, 100}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
