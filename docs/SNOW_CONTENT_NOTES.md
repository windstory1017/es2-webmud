# Snow content notes, upstream-only

`/d/snow` is explicitly documented by upstream as the human starting town.

Observed content includes 43 top-level room/area LPC files, 23 NPC LPC files, three area object files, and one area-specific skill (`dragon_force.c`). This is only a directory inventory. The generated catalog after cloning upstream remains canonical for counts.

## Representative behaviors that tooling must preserve
- `teacher.c`: 王懷芝, human level 5, literate 60; has chat-driven teaching plus custom `sit` action and learning state.
- `smith.c`: 鐵匠, villager + vendor, level 13; merchandise mapping and custom vendor/list behavior; also accepts a specific object for a scripted conversion path.
- `guard.c`: 青衣漢子, level 20, explicit sword/blade/parry/unarmed/dodge skills, equipped stock armor/longsword, and overrides `go` to guard an eastward entrance conditionally.
- `dragon_force.c`: a real skill daemon. It enables as `force`, has custom skill-improvement thresholds, stat advancement, exercise/busy behavior, resource consumption and skill improvement.

These are why the editor uses raw LPC as canonical and catalogs as an index. Converting these objects to flat JSON would throw away gameplay logic.
