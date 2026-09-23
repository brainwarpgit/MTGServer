# Vehicle deed create commands

Updated: 2026-09-23. Source: current server Lua registrations and configured TRE
assets with the `mtg_patch_024` overlay.

## Complete creation chains

54 active deed paths: 37 standard and 17 TCG. Each resolves to a distinct mobile
template. The 12 disabled duplicate deed registrations are excluded; their
vehicle and control-device templates remain available for existing vehicles.
Static checks pass. On 2026-09-23, the user reported that the active vehicles
work in game and confirmed the corrected template-inspection labels.
The regular AT-RT deed is disabled after the user reported its appearance still
was not fixed. The camouflage AT-RT remains available.

The X-31 and X-34 share a visible model but have different speed and acceleration
settings, so both remain listed. The black Swoop Racer now has a complete
vehicle setup using the supplied black texture and standard Swoop performance.
The user confirmed it working on 2026-09-23 after a further texture adjustment.

| Vehicle / deed display name | Create command |
| --- | --- |
| A1 Deluxe Floater | `/object createitem object/tangible/deed/vehicle_deed/a1_deluxe_floater_deed.iff` |
| AB-1 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_ab1_deed.iff` |
| Advanced STAP-1 | `/object createitem object/tangible/deed/vehicle_deed/speeder_stap_deed.iff` |
| Advanced Tantive IV Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_tantive4_adv_deed.iff` |
| Air-2 Racing Swoop | `/object createitem object/tangible/tcg/series5/vehicle_deed_air2_swoop_speeder.iff` |
| AT-PT Walker | `/object createitem object/tangible/deed/vehicle_deed/atpt_deed.iff` |
| AT-RT Walker (Camouflage) | `/object createitem object/tangible/deed/vehicle_deed/temp_walker1_deed.iff` |
| AT-ST Walker | `/object createitem object/tangible/deed/vehicle_deed/atst_deed.iff` |
| AT-XT Walker | `/object createitem object/tangible/deed/vehicle_deed/atxt_deed.iff` |
| AV-21 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_av21_deed.iff` |
| BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_deed.iff` |
| Basilisk War Droid | `/object createitem object/tangible/deed/vehicle_deed/basilisk_war_droid.iff` |
| BT310 Podracer | `/object createitem object/tangible/tcg/series4/vehicle_deed_balta_podracer.iff` |
| Cloud City Pod Car | `/object createitem object/tangible/deed/vehicle_deed/tcg_8_air_speeder_deed.iff` |
| Desert Skiff | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_desert_skiff_deed.iff` |
| FG 8T8-Twin Block2 Special Podracer | `/object createitem object/tangible/tcg/series5/vehicle_deed_fg_8t8_podracer.iff` |
| Flare-S Swoop | `/object createitem object/tangible/deed/vehicle_deed/flare_s_swoop.iff` |
| Flash Speeder | `/object createitem object/tangible/deed/vehicle_deed/speederbike_flash_deed.iff` |
| Geonosian Speeder | `/object createitem object/tangible/tcg/series4/vehicle_deed_geonosian_speeder.iff` |
| GPE-3130 Podracer | `/object createitem object/tangible/tcg/series1/vehicle_deed_podracer_mawhonic.iff` |
| HK-47 Jetpack | `/object createitem object/tangible/tcg/series6/deed_tcg_hk47_jetpack.iff` |
| Hover Chair | `/object createitem object/tangible/deed/vehicle_deed/hover_chair_deed.iff` |
| Hover Lifter Speeder | `/object createitem object/tangible/deed/vehicle_deed/hoverlifter_speeder.iff` |
| Imperial Command BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_imperial_deed.iff` |
| IPG-X1131 Longtail Podracer | `/object createitem object/tangible/tcg/series3/vehicle_deed_podracer_longtail.iff` |
| Koro-2 Exodrive Airspeeder | `/object createitem object/tangible/tcg/series3/vehicle_deed_koro2_exodrive_airspeeder.iff` |
| Lava Skiff | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_lava_skiff_deed.iff` |
| Mandalorian Jetpack | `/object createitem object/tangible/deed/vehicle_deed/jetpack_deed.iff` |
| Mechno-Chair | `/object createitem object/tangible/tcg/series1/vehicle_deed_mechno_chair.iff` |
| Merr-Sonn JT-12 Jetpack | `/object createitem object/tangible/tcg/series3/deed_tcg_merr_sonn_jt12_jetpack.iff` |
| Mustafarian Panning Droid | `/object createitem object/tangible/deed/vehicle_deed/mustafar_panning_droid.iff` |
| Ord Pedrovia Podracer | `/object createitem object/tangible/tcg/series1/vehicle_deed_podracer_gasgano.iff` |
| Radon-Ulzer Podracer | `/object createitem object/tangible/deed/vehicle_deed/podracer_anakin_deed.iff` |
| RAM-1511 Transport | `/object createitem object/tangible/deed/vehicle_deed/military_transport_deed.iff` |
| Rebel Command BARC Speeder | `/object createitem object/tangible/deed/vehicle_deed/barc_speeder_rebel_deed.iff` |
| Republic Gunship | `/object createitem object/tangible/tcg/series7/combine_reward_deed_republic_gunship.iff` |
| RIC-920 Rickshaw | `/object createitem object/tangible/deed/vehicle_deed/speeder_ric_920_deed.iff` |
| Senate Pod | `/object createitem object/tangible/tcg/series4/vehicle_deed_senate_pod.iff` |
| Sith Speeder | `/object createitem object/tangible/tcg/series1/vehicle_deed_sith_speeder.iff` |
| Speeder Bike | `/object createitem object/tangible/deed/vehicle_deed/speederbike_deed.iff` |
| Swamp Speeder | `/object createitem object/tangible/tcg/series3/vehicle_deed_swamp_speeder.iff` |
| Swoop Bike | `/object createitem object/tangible/deed/vehicle_deed/speederbike_swoop_deed.iff` |
| Swoop Racer (Black) | `/object createitem object/tangible/deed/vehicle_deed/swoopracer_black_deed.iff` |
| T-47 Snowspeeder | `/object createitem object/tangible/deed/vehicle_deed/snowspeeder_deed.iff` |
| Tantive IV Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_tantive4_deed.iff` |
| TSMEU-6 Personal Wheel Bike | `/object createitem object/tangible/deed/vehicle_deed/grievous_wheel_bike_deed.iff` |
| USV-5 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_usv5_deed.iff` |
| USV-5 Modified Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_usv5_s02_deed.iff` |
| V-35 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_v35_deed.iff` |
| X-31 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_x31_deed.iff` |
| X-34 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_x34_deed.iff` |
| XJ-2 Airspeeder | `/object createitem object/tangible/tcg/series1/vehicle_deed_organa_speeder.iff` |
| XJ-6 Airspeeder | `/object createitem object/tangible/tcg/series5/vehicle_deed_xj6_air_speeder.iff` |
| XP-38 Landspeeder | `/object createitem object/tangible/deed/vehicle_deed/landspeeder_xp38_deed.iff` |

## Deferred vehicle entries

The regular `atrt_deed.iff` registration was disabled on 2026-09-23 at the user's
request. Its vehicle/control-device templates and assets are retained for saved
vehicles and later work; its spawn command is removed from this list.

The unfinished AT-AT and blue, gold, green, purple, red, and silver Swoop Racer
deed registrations were disabled on 2026-09-23 at the user's request. They have
been removed from this command list until the user resumes work on their missing
vehicle definitions/assets. Their source files are retained for that work.

The two light-bending deeds are retired and have no active create commands.
Base templates, prototype components, and instant-travel items are not deployable
vehicle deeds and are excluded.
