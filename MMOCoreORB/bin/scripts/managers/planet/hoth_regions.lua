-- Planet Region Definitions
--
-- {"regionName", x, y, shape and size, tier, {"spawnGroup1", ...}, maxSpawnLimit}
-- For circle and ring, x and y are the center point
-- For rectangles, x and y are the bottom left corner. x2 and y2 (see below) are the upper right corner
-- Shape and size is a table with the following format depending on the shape of the area:
--   - Circle: {CIRCLE, radius}
--   - Rectangle: {RECTANGLE, x2, y2}
--   - Ring: {RING, inner radius, outer radius}
-- Tier is a bit mask with the following possible values where each hexadecimal position is one possible configuration.
-- That means that it is not possible to have both a spawn area and a no spawn area in the same region, but
-- a spawn area that is also a no build zone is possible.

require("scripts.managers.planet.regions")

hoth_regions = {
	-- Named Regions, POIs and Decor

	-- Cities
	{"@hoth_region_names:frostline_outpost", 671, 2035, {CIRCLE, 260}, CITY + NOSPAWNAREA + NOBUILDZONEAREA + NAVAREA},
	{"@hoth_region_names:south_range_outpost", 0, -2000, {CIRCLE, 250}, CITY + NOSPAWNAREA + NOBUILDZONEAREA},

	--Spawns
}
