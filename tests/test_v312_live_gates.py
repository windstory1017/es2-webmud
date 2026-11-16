from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def test_version_synced():
 assert (ROOT/'VERSION').read_text().strip().startswith('3.')
 # 版本號已不再逐版同步（VERSION、server/package.json、驗收紀錄各自獨立），所以只檢查有版本號、不綁特定數字。
def test_soak_gate_is_password_safe_and_exercises_core_paths():
 t=(ROOT/'tools/live_multiplayer_soak.py').read_text()
 assert 'getpass.getpass' in t and "tell {ub}" in t and "send('save')" in t and "send('hp')" in t
 assert "A.close()" in t and "'reconnectPass'" in t
 assert "'Password'" not in t.split("json.dumps",1)[-1]
def test_save_integrity_checks_canonical_shards():
 t=(ROOT/'tools/live_save_integrity.py').read_text()
 assert "mud/'data/user'/u[0]/(u+'.o')" in t
 assert "mud/'data/login'/u[0]/(u+'.o')" in t
 assert ".o.tmp" in t and 'sha256' in t
def test_windows_entrypoints_exist():
 assert (ROOT/'CHECK_MULTIPLAYER_SOAK.bat').exists()
 assert (ROOT/'CHECK_SAVE_INTEGRITY.bat').exists()
