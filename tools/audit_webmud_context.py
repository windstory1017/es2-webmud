from pathlib import Path
import json,sys
ROOT=Path(__file__).resolve().parents[1]
M=ROOT/'source/upstream/mudlib'
app=(ROOT/'web/app.js').read_text(encoding='utf-8')
css=(ROOT/'web/styles.css').read_text(encoding='utf-8')
hud=(M/'cmds/usr/webhud.c').read_text(encoding='utf-8')
inn=(M/'d/snow/inn_hall.c').read_text(encoding='utf-8')
checks={
 'room_scoped_context_parser':'contextRoomScan' in app and 'if(!contextRoomScan)continue' in app,
 'ansi_stripped_for_context':'const text=cleanText(String(raw))' in app,
 'aligned_combat_values':'context-combat-stat-head' in css and 'font-variant-numeric:tabular-nums' in css and 'grid-template-columns:34px 1fr' in css,
 'inventory_hud_feed':'@@WEBHUD|INVBEGIN' in hud and '@@WEBHUD|ITEM|' in hud,
 'silent_hud_framing':'@@WEBHUD|BEGIN' in hud and '@@WEBHUD|END' in hud and 'hasEnd' in app,
 'live_hud_precedes_score_cache':app.find('const live=observer.state.vitals') < app.find('const detail=observer.state.scoreDetail'),
 'hud_poll_800ms':'const HUD_POLL_MS = 800;' in app,
 'hud_player_priority':'HUD_USER_GRACE_MS = 350' in app and 'lastUserCommandAt' in app and 'kickHudAfterServerText' in app,
 'hud_control_hidden':"if(/^@@WEBHUD\\|/.test(plain))return false;" in app and "if(/^webhud$/i.test(plain))return false;" in app,
 'food_button':'return "eat"' in hud and "eat:'吃'" in app,
 'drink_button':'return "drink"' in hud and "drink:'喝'" in app,
 'equipment_buttons':'return "wear"' in hud and 'return "wield"' in hud and "wear:'裝備'" in app,
 'buttons_send_server_commands':"ws.send(command+'\\r\\n')" in app,
 # 測試用黃金已依使用者要求移除（2026-10-23）
 'test_gold_removed':'/custom/test/obj/infinite_100_gold' not in inn and not (M/'custom/test/obj/infinite_100_gold.c').exists(),
}
fail=[k for k,v in checks.items() if not v]
out={'version':(ROOT/'VERSION').read_text().strip(),'checks':checks,'failures':fail,'passed':not fail}
(ROOT/'reports/webmud_context_audit.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(out,ensure_ascii=False,indent=2))
sys.exit(1 if fail else 0)
