# ES2 WebMUD — handoff notes (this WSL checkout)

**User is non-technical** — plain language, no unexplained jargon, reply in Traditional
Chinese. See project charter in this session's memory: preserve original ES2, tag
everything ORIGINAL/DOCUMENTED/INFERRED/NEW/MODIFIED, never invent historical data.

## What this project is

`github.com/wolfer168/es2-webmud` — a 3-person collaboration (user + 2 others) building
a web/multiplayer front end for ES2 (東方故事2 / 天朝帝國). Workflow is in the repo's own
`GITHUB_三人協作說明.txt`: pull → edit your area → test locally via
`啟動單機測試版.bat` (Windows) → commit → push. The repo enforces its own provenance
rule via `source/SOURCE_POLICY.json`: game data/rules may **only** come from
`github.com/taedlar/es2_mudlib` main branch — nothing else, no exceptions, checked by
`tools/verify_source.py`.

## ⚠️ Correction, later on 2026-09-16 — read this first

The initial setup below made a mistake: it reset `source/upstream` to a byte-pure
`taedlar/es2_mudlib` checkout, which **deleted the ~116-file custom content layer**
(`mudlib/custom/{home,oldpine,oldpine_recovered,race,snow,test,wizroom,zhenwu}/` plus
documented modifications to canonical files — see `CONTENT_PATCH_TRANSACTION.md` for the
full v3.20.x changelog, current VERSION is `3.20.77 MAP V7 READONLY WORLD`). This repo is
**not** a pure taedlar mirror by design — it's taedlar base + an approved custom layer.
`tools/verify_source.py` / `tools/build_catalog.py` check for byte-identity to pure
taedlar `main` and will **always** fail against the real project state — that's expected,
don't chase it as a bug.

**Fixed** via `git checkout HEAD -- source/upstream` at this outer repo's root (nothing
had been committed/pushed in between, so recovery was clean). Rebuilt Neolith for Linux
from this tree's own vendored `neolith/` source (not reused from elsewhere — driver
source can differ): `cd source/upstream/neolith && cmake --preset linux && cmake --build
--preset ci-linux`. Recreated `mudlib/log/` and `mudlib/data/{login,user,mail}/<a-z>/`
(gitignored runtime dirs, not part of the commit).

**Also:** `neolith.conf` here listens on port **4001**, not 4000 — `es2webmud_ctl.sh`
sets `MUD_PORT=4001` for the bridge accordingly.

**Verified live**: scripted a full login + character creation, landed in the modified
`d/snow/inn_hall.c`, saw the custom `custom/home/npc/home_teleporter.c` NPC
("家園傳送師") and `custom/test/obj/infinite_100_gold.c` ("測試用黃金") right there —
proof the custom layer is actually loaded and running. Test character deleted after.

**Still broken / next up:** `catalog/generated/*.json` was built once against the WRONG
(pure-taedlar) tree before this fix, and hasn't been rebuilt against the correct one
since `build_catalog.py` hard-fails at its `verify_source.py` gate. Doesn't block
gameplay (Neolith doesn't need the catalog), but the web admin/workbench dashboards
reading those JSON files are stale/wrong. Needs either a way to run the indexer steps
(`tools/index_es2.py` etc.) directly, skipping the verify gate, or a `verify_source.py`
change that understands "taedlar + approved custom" as a valid state.

## This WSL checkout's state (set up 2026-09-16)

- `source/upstream` = a clean, verified checkout of `taedlar/es2_mudlib` @ `cc45c06`
  (main), reusing an already-built Neolith binary at
  `source/upstream/neolith/out/build/linux/src/RelWithDebInfo/neolith` — saved a full
  rebuild. `python3 tools/verify_source.py source/upstream` → `ok: true`.
- Catalog rebuilt: `python3 tools/build_catalog.py source/upstream -o catalog/generated`
  → 513 files indexed, all 12 required core runtime files present.
- **Nothing has been committed or pushed.** Swapping in the clean upstream changed
  ~1155 files at the outer repo's git-status level (this local copy vs whatever was
  previously committed there) — that's expected since the vendored copy wasn't a
  submodule, just files. Do NOT `git add -A && commit` this without the user
  deliberately deciding to, since two teammates share this repo. If asked to sync it,
  review the diff first and coordinate with the user about their teammates.
- Bridge: `server/index.js` (plain Node + `ws`, ESM). Config via env vars `PORT`
  (default 8080), `MUD_HOST`/`MUD_PORT` (default 127.0.0.1:4000), `ES2_REPO` (upstream
  path). **WebSocket endpoint is `/mud`, not `/`** — a bare `ws://host:8080` connect
  gets HTTP 400. Static site root is `web/`, served at `/`.
- Control script: **`~/es2-webmud/es2webmud_ctl.sh {start|stop|status}`**. Uses
  `setsid ... & disown` for both Neolith and the bridge — plain `&` did not survive
  across separate `wsl.exe bash -lc` invocations from the Windows-side session (each
  call is its own attach; a bare background job could get killed when it ended).
  `stop` kills by binary path pattern AND by port (`fuser -k 8080/tcp` /
  `4000/tcp`) since a relative-path launch (`node index.js` from inside `server/`)
  won't match a path-based `pkill` pattern reliably.
- Verified end-to-end: `curl http://127.0.0.1:8080/api/health` → `ok:true`; a raw
  `ws://127.0.0.1:8080/mud` client receives the real ES2 login banner.
- Runtime data dirs were pre-created under `source/upstream/mudlib/{log,data/{login,user,mail}/<a-z>}`
  (same fix as the old taedlar setup needed — Neolith's `save_object()`/`assure_file()`
  don't auto-create parent dirs, and `securityd.c` only whitelists `mkdir` for specific
  cases). This is a deployment-step workaround, not a code change — not committed.

## What was retired

- Old standalone `~/es2_mudlib` (taedlar clone + Neolith, with an uncommitted
  `MODIFIED` `NET_DEAD_TIMEOUT` change + changelog + a `reference/能力值與強度.txt`)
  and `~/es2-web-bridge` (the original hand-rolled ws bridge) — **archived in full** to
  `~/es2-archive/20260916-100538/` before removal, then removed. `~/es2_mudlib` could
  not be fully deleted (one file under `reference/` is root-owned from an earlier
  session that ran as root — `sudo rm -rf ~/es2_mudlib` would finish it; harmless as-is).
- `~/es2-utf8` (old FluffOS experiment) and `~/es2-reference/*` (es2tips / japentery /
  vovo2000 archives — see this session's `es2tips-archive` memory) are untouched,
  unrelated to this pivot.
- Noticed `~/新種族設定表.xlsx` sitting loose in the WSL home dir (not in any project
  folder) — flagged to the user, not moved.

## Next steps (not yet done)

- Decide with the user (and their teammates) whether/how to sync this verified
  `source/upstream` state back to GitHub.
- Read `README.md`, `docs/`, and `MULTIPLAYER_STATUS.md` in this repo for the fuller
  design (HUD, map, chat, catalog workbench) before making content changes.
- Figure out a Windows-side equivalent for teammates who use the `.bat`/`.ps1` scripts,
  vs. this WSL path — they don't need to match, just both need to pass
  `verify_source.py` against the same canonical commit before anyone's changes get
  merged.
