-- City circles match the client region table for names and enter/leave boundaries.
-- Protect settled areas from random spawns and player construction.

require("scripts.managers.planet.regions")

mustafar_regions = {
	{"@mustafar:mensix", -1100, 2365, {CIRCLE, 400}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},
}
