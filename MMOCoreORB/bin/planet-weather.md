# Planet weather

Updated 2026-09-25. Committed on 2026-09-25. Standalone checks pass, and the user confirmed
that weather works in game, closing the initial runtime check.

## Coverage and settings

Weather is enabled for **all 19 travel planets**, plus Kashyyyk's Hunting
Grounds, Dead Forest, and Rryatt Trail: **22 outdoor scenes**. The ten original
planets retain their existing weather settings. Kachirho also retains its
existing profile; the other outdoor scenes now have explicit climate profiles.

Two server files control this:

- `scripts/managers/planet/planet_manager.lua`: starts each scene's weather manager.
- `scripts/managers/weather_manager.lua`: supplies that scene's timing and stability.

Higher stability favors calmer weather; lower stability allows more frequent
and stronger storms. The duration is a tuning input in seconds, not a fixed
time between storms. The current engine chooses roughly 0.5–1.5 times that
duration and divides it by the base intensity plus one. Local storm patterns
also vary over the terrain and during their lifetime.

| Scene | Stability | Duration | Intended character |
| --- | ---: | ---: | --- |
| Chandrila | 95 | 5400 | Long fair periods, occasional showers |
| Corellia | 90 | 3600 | Existing temperate climate |
| Coruscant | 92 | 5400 | Mostly calm city skies, intermittent rain |
| Dantooine | 85 | 3600 | Existing grassland climate |
| Dathomir | 50 | 3600 | Existing unsettled wilderness |
| Endor | 90 | 3600 | Existing forest climate |
| Hoth | 45 | 2400 | More active snow and blizzard weather |
| Dromund Kaas (`kaas`) | 35 | 1800 | Frequent, changeable jungle storms |
| Kashyyyk Dead Forest | 60 | 3000 | Unsettled forest conditions |
| Kashyyyk Hunting Grounds | 65 | 3000 | Regular passing storms |
| Kashyyyk Main / Kachirho | 70 | 3600 | Existing coastal forest profile |
| Kashyyyk Rryatt Trail | 65 | 3000 | Changing forest conditions |
| Lok | 60 | 3600 | Existing dust-storm climate |
| Mandalore | 85 | 3600 | Mostly settled weather, occasional squalls |
| Moraband | 80 | 5400 | Long dry periods, occasional dust storms |
| Mustafar | 80 | 3600 | Changing volcanic haze and drifting embers |
| Naboo | 90 | 3600 | Existing temperate climate |
| Rori | 80 | 3600 | Existing marshland climate |
| Taanab | 88 | 4200 | Mostly fair rural weather with showers |
| Talus | 80 | 3600 | Existing temperate climate |
| Tatooine | 70 | 3600 | Existing sandstorm climate |
| Yavin 4 | 70 | 3600 | Existing jungle climate |

All profiles explicitly disable damaging sandstorms. Storms are atmospheric;
they do not add the optional damage, blindness, knockdown, or dismount effects.

Tutorial and the five dungeon scenes retain disabled weather. The separate
combined `kashyyyk` scene remains unconfigured: creating its first planet
configuration would also load its previously skipped 4,679-object snapshot.
Kashyyyk travel uses `kashyyyk_main`, which has weather. Activating the combined
world's content is separate from configuring the existing outdoor areas.

## Client effects

Server weather IDs 0–4 represent clear through extreme intensity. The client's
environment table decides what players see, rather than the server specifying
"rain" or "snow."

- Hoth has actual snow particle assets. Some authored environment families use
  persistent snow even when the server chooses a calmer intensity. Frostline's
  terrain region supports the full clear-to-heavy-snow sequence.
- Moraband has dust and heavy-dust effects.
- Chandrila, Coruscant, Kaas, Mandalore, and Taanab have rain definitions.
- Kashyyyk uses regional effects: open areas can show rain, forest regions use
  drifting particles, and the lower Rryatt areas suppress precipitation.
- Mustafar had no `datatables/environment/mustafar.iff` in any of the 81 available
  archives. The prepared replacement supplies the terrain's environment families
  and five weather intensities using existing Mustafar visual assets. It also
  replaces generic fallback lighting with the authored Mustafar color palettes;
  future visual regression checks should cover daytime and nighttime appearance.
  Its 55 rows cover 11 terrain families: intensity 0 is clear, 1 adds light haze,
  and 2–4 add floating embers with stronger haze, wind, and cloud movement. The
  ember effect keeps its original density and 20-particle cap at levels 2–4.

The audit also found existing missing Moraband sky-gradient/music references,
missing Coruscant ambient music, and a Kashyyyk elder-music reference lacking
its `.snd` suffix. Those existing environment entries are unchanged. The
precipitation effects used by the configured climates have their dependencies.

The existing engine immediately generates a weather pattern at startup, so
`defaultWeather` does not reliably force the initial state. The old comments
about transition timers and a special startup value of 5 did not describe the
current implementation; the configuration comments now describe the actual code.

## Climate references and design choices

The custom profiles take broad inspiration from official descriptions:
[Hoth's frozen landscape](https://www.starwars.com/databank/hoth),
[Chandrila's temperate hills](https://www.starwars.com/databank/chandrila),
[Coruscant's temperate artificial environment](https://www.starwars.com/databank/coruscant),
[Kashyyyk's forests and swamps](https://www.starwars.com/databank/kashyyyk),
[Moraband's hot red desert](https://www.starwars.com/databank/moraband), and
[Mustafar's volcanic landscape](https://www.starwars.com/databank/mustafar).
The [official Dromund Kaas description](https://www.swtor.com/holonet/planets/dromund-kaas)
describes a stormy jungle atmosphere in the Old Republic era.

The exact stability and timing numbers are gameplay choices. Mandalore follows
the existing MTG landscape; this is an adaptation rather than a reproduction of
the desert world in its [modern official depiction](https://www.starwars.com/databank/mandalore).
Taanab's mild rural profile is also a project interpretation.

## Deployment and regression reference

The user confirmed working weather on 2026-09-24. The following instructions
remain useful when deploying or tuning future climate changes.

Deploy both Lua files and restart the server so the newly enabled managers are
created. Package the new Mustafar environment table from `mtg_patch_024` and
install the matching patch on clients and server. No Core3 rebuild is needed
for these weather changes. The earlier travel-command change has its own build
requirement, documented in [planet-travel.md](planet-travel.md).

For an administrator with level 15 or higher, use these commands while outdoors
on the planet being checked:

```text
/server weather info
/server weather change 0
/server weather change 2
/server weather change 4
/server weather enable
```

Check each forced intensity before issuing the next command. `change` temporarily
stops automatic weather for the whole scene; finish with `enable` to resume it.
The server checks client weather about every three seconds, and visual effects
may take longer to settle. Compare open ground and forest/city regions, and
check indoor shelter behavior. Keep these as regression checks for future
climate changes. The confirmation does not record every
intensity, terrain family, or day/night combination individually. Codex did not
build or run Core3 for this work.

Standalone validation passed for all 22 profiles, Lua field/range correctness,
positive scheduling intervals, preservation of existing climate values, and
unchanged travel data. Existing Hoth, Mustafar, and custom-planet travel checks
also pass. Mustafar's client table passes schema, unique-key, terrain-family,
and 26 asset-dependency checks; its original terrain and source sky assets are unchanged.

## Mustafar shadow-format correction — 2026-09-24

The supplied screenshot at approximately **X -9, height 137, Y 11** shows pale,
solid projected character silhouettes. Read-only inspection of the user's actual
`SWGEmu.exe` confirmed that its lighting-ramp validator accepts **256×8** pixels
and rejects Mustafar's five authored **256×10** ramps. The rejection path selects
an **opaque gray** shadow color, explaining the bright fallback visible in the
screenshot. The earlier static audit used newer-client format rules and missed
this incompatibility.

Prepared legacy-format copies under `mtg_patch_024/terrain/colorramp/`:

- `mustafar_global.tga`
- `mustafar_cool.tga`
- `mustafar_hot.tga`
- `mustafar_hot_02.tga`
- `mustafar_poison.tga`

Each is now **256×8, 32-bit BGRA**. All original logical channels 0–7 are preserved
byte-for-byte, including TGA orientation; only the two newer lighting channels
are removed. This restores the authored dark, translucent shadow colors rather
than changing weather values or guessing new lighting colors. Terrain, sun/time
settings, clouds, fog and travel data remain unchanged.

Validation passed against the actual client's dimension checks and opaque-gray
fallback, with **10,240 retained pixels** compared independently and **75 unrelated
ramps** preserved. The Mustafar weather regression still passes for **55 rows and
26 asset dependencies**. The executable was neither changed nor run.

**Deployment/testing:** include these five files in the replacement TRE and
fully restart the client. No Core3 rebuild is required. Revisit the reported
location and check the shadow color on both flat ground and steep slopes.
The user subsequently confirmed that Mustafar shadows now look okay. That
closes the reported pale-shadow issue; no further lighting changes are made.
Keep varied slopes/weather as regression coverage if artifacts recur rather
than assuming every client shadow case was individually tested.
Committed on 2026-09-25; no Core3 build or run performed.
