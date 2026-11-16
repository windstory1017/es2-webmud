from pathlib import Path
import json
ROOT=Path(__file__).resolve().parents[1]

def text(p): return (ROOT/p).read_text(encoding='utf-8')

def test_version_and_ledger():
 version=text('VERSION').strip()
 assert version
 # 版本號已不再逐版同步（VERSION、server/package.json、驗收紀錄各自獨立），所以只檢查有版本號、不綁特定數字。
 led=json.loads(text('MULTIPLAYER_ACCEPTANCE_LEDGER.json'))
 assert 'mutual player fight in a canonical combat-allowed room' in led['pendingLive']
 assert 'live kill/PK' in led['withheldDestructive']

def test_npc_combat_no_longer_claims_nonlethal():
 py=text('tools/live_multiplayer_combat_acceptance.py')
 bat=text('CHECK_MULTIPLAYER_COMBAT_LIVE.bat')
 assert 'TYPE RISKY FIGHT' in py
 assert 'kill_ob()' in py
 assert 'CONDITION NOT MET' in py
 assert 'POTENTIALLY RISKY' in bat
 assert 'non-lethal NPC combat acceptance passed' not in bat

def test_preferred_pvp_gate_is_mutual_fight_only():
 py=text('tools/live_multiplayer_pvp_fight.py')
 bat=text('CHECK_MULTIPLAYER_PVP_FIGHT_LIVE.bat')
 assert "A.send('fight '+ub)" in py
 assert "B.send('fight '+ua)" in py
 assert "send('kill" not in py
 assert "A.send('halt')" in py and "B.send('halt')" in py
 assert 'TYPE PVP FIGHT' in py
 assert 'NEVER `kill`' in py
 assert 'NON-KILL' in bat

def test_sharedworld_does_not_use_npc_fight_and_item_race_uses_get_failure():
 py=text('tools/live_multiplayer_sharedworld.py')
 assert "A.send('fight '+npc)" not in py
 assert "B.send('fight '+npc)" not in py
 assert "A.send('look '+npc)" in py
 assert '你附近沒有這樣東西' in py
 assert 'single winning owner under concurrent get' in py
 assert "item.lower() in ia.lower()" not in py

def test_restart_requires_offline_before_snapshot():
 assert 'Save and LOG OUT both test characters first.' in text('CHECK_RESTART_BEFORE.bat')
 assert 'both were offline' in text('CHECK_RESTART_AFTER.bat')

def test_master_labels_risky_and_withheld_gates():
 py=text('tools/multiplayer_master_gate.py')
 assert 'CHECK_MULTIPLAYER_PVP_FIGHT_LIVE.bat' in py
 assert 'ASSISTED-RISKY' in py
 assert 'WITHHELD-DESTRUCTIVE' in py


def test_live_infrastructure_reports_use_current_version():
 storm=text('tools/live_neolith_storm.py')
 endurance=text('tests/test_multiplayer_endurance_v317.mjs')
 assert "'version':'3.17.0'" not in storm
 assert "version:'3.17.0'" not in endurance
 assert "root/'VERSION'" in storm
 assert "../VERSION" in endurance
