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


--------------------------
-- Weather System Notes
--------------------------
-- Enable each outdoor scene in planet/planet_manager.lua with weatherEnabled = 1.
-- This file supplies that scene's climate profile, using the exact scene name.
-- Weather IDs 0-4 run from clear to extreme. The client's environment table
-- chooses the actual effect (rain, snow, dust, or regional forest effects).
-- The server combines a random base intensity with a moving local storm map.
--
-- defaultWeather: initial base, 0-4. Initialization immediately generates an
-- automatic weather pattern, so this does not force the weather at startup.
-- averageWeatherDuration: positive duration input in seconds. Actual patterns
-- last roughly 0.5-1.5 times this value, divided by (base intensity + 1).
-- weatherStability: 0-100. Higher values favor calmer bases and milder patterns;
-- lower values allow more frequent and stronger storms. Even 100 is not a
-- permanent-clear setting. Patterns vary by position and time.
--
-- hasDamagingSandstorms: 0 or 1. These climate profiles use atmospheric effects
-- only. Setting this to 1 enables damage, blindness and possible knockdowns or
-- dismounts during extreme weather. sandstormDamage is read only when enabled.
-- See bin/planet-weather.md for climate choices, client assets and verification.

-- Temperate hills: long fair periods with occasional showers.
chandrila = {
	defaultWeather = 0,
	averageWeatherDuration = 5400,
	weatherStability = 95,
	hasDamagingSandstorms = 0,
}

-- Temperate plains and cities: established Core3 climate.
corellia = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 90,
	hasDamagingSandstorms = 0,
}

-- Urban skies: long calm periods and intermittent rain.
coruscant = {
	defaultWeather = 0,
	averageWeatherDuration = 5400,
	weatherStability = 92,
	hasDamagingSandstorms = 0,
}

-- Open grasslands: established Core3 climate.
dantooine = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 85,
	hasDamagingSandstorms = 0,
}

-- Unsettled wilderness: established Core3 climate.
dathomir = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 50,
	hasDamagingSandstorms = 0,
}

-- Forest moon: established Core3 climate.
endor = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 90,
	hasDamagingSandstorms = 0,
}

-- Ice world: frequent snow patterns with breaks between blizzards.
hoth = {
	defaultWeather = 0,
	averageWeatherDuration = 2400,
	weatherStability = 45,
	hasDamagingSandstorms = 0,
}

-- Dromund Kaas: frequent, changeable jungle storms.
kaas = {
	defaultWeather = 0,
	averageWeatherDuration = 1800,
	weatherStability = 35,
	hasDamagingSandstorms = 0,
}

-- Damp forest floor: more unsettled regional weather.
kashyyyk_dead_forest = {
	defaultWeather = 0,
	averageWeatherDuration = 3000,
	weatherStability = 60,
	hasDamagingSandstorms = 0,
}

-- Exposed hunting grounds: regular passing storms.
kashyyyk_hunting = {
	defaultWeather = 0,
	averageWeatherDuration = 3000,
	weatherStability = 65,
	hasDamagingSandstorms = 0,
}

-- Kachirho and coastal forest: established regional climate.
kashyyyk_main = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 70,
	hasDamagingSandstorms = 0,
}

-- Forest trails: changing conditions; effects follow each terrain region.
kashyyyk_rryatt_trail = {
	defaultWeather = 0,
	averageWeatherDuration = 3000,
	weatherStability = 65,
	hasDamagingSandstorms = 0,
}

-- Arid wilderness: established Core3 dust-storm climate.
lok = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 60,
	hasDamagingSandstorms = 0,
	sandstormDamage = 50, -- Used only if damaging sandstorms are enabled.
}

-- MTG landscape: mostly settled conditions with occasional squalls.
mandalore = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 85,
	hasDamagingSandstorms = 0,
}

-- Red desert: long dry periods punctuated by dust storms.
moraband = {
	defaultWeather = 0,
	averageWeatherDuration = 5400,
	weatherStability = 80,
	hasDamagingSandstorms = 0,
}

-- Volcanic world: changing haze and drifting embers.
mustafar = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 80,
	hasDamagingSandstorms = 0,
}

-- Temperate lakes and plains: established Core3 climate.
naboo = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 90,
	hasDamagingSandstorms = 0,
}

-- Marshland: established Core3 climate.
rori = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 80,
	hasDamagingSandstorms = 0,
}

-- Rural plains: mostly fair conditions with periodic showers.
taanab = {
	defaultWeather = 0,
	averageWeatherDuration = 4200,
	weatherStability = 88,
	hasDamagingSandstorms = 0,
}

-- Temperate wilderness: established Core3 climate.
talus = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 80,
	hasDamagingSandstorms = 0,
}

-- Desert world: established Core3 sandstorm climate.
tatooine = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 70,
	hasDamagingSandstorms = 0,
	sandstormDamage = 70, -- Used only if damaging sandstorms are enabled.
}

-- Jungle moon: established Core3 climate.
yavin4 = {
	defaultWeather = 0,
	averageWeatherDuration = 3600,
	weatherStability = 70,
	hasDamagingSandstorms = 0,
}
