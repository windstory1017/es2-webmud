#!/usr/bin/env python3
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
app=(root/'web/app.js').read_text()
hud=(root/'source/upstream/mudlib/cmds/usr/webhud.c').read_text()
g=json.loads((root/'web/custom_map_graph.json').read_text())
ids={n['id'] for n in g['nodes']}
checks={
'exact_room_from_lpc': '@@WEBHUD|ROOM|' in hud and 'base_name(environment(me))' in hud,
'browser_uses_exact_room': "a[1]==='ROOM'" in app and ('setExactRoom(a[2]' in app or 'beginRuntimeRoomSnapshot(a[2]' in app),
'ambiguous_labels_not_guessed': 'return null;' in app,
'custom_graph_loaded': "fetch('/custom_map_graph.json')" in app,
'oldpine_in_map': '/custom/oldpine/room/path01' in ids and '/custom/oldpine/room/path10' in ids,
'zhenwu_in_map': '/custom/zhenwu/room/gate' in ids and '/custom/zhenwu/room/yard14' in ids,
'home_in_map_index': '/custom/home/room/home_hall' in ids,
# recall／recall home 已依使用者要求取消（2026-10-23）
'recall_removed': not (root/'source/upstream/mudlib/cmds/usr/recall.c').exists(),
}
out={'version':(root/'VERSION').read_text().strip(),'checks':checks,'failures':[k for k,v in checks.items() if not v],'passed':all(checks.values())}
(root/'reports/local_map_runtime_audit.json').write_text(json.dumps(out,ensure_ascii=False,indent=2)+"\n")
print(json.dumps(out,ensure_ascii=False,indent=2))
raise SystemExit(0 if out['passed'] else 1)
