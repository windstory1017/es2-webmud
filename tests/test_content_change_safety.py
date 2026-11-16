from pathlib import Path
import subprocess,sys,json
ROOT=Path(__file__).resolve().parents[1]
def test_snapshot_rejects_escape():
 r=subprocess.run([sys.executable,str(ROOT/'tools/content_transaction_snapshot.py'),'../../VERSION'],cwd=ROOT,text=True,capture_output=True)
 assert r.returncode!=0 and 'REJECT outside canonical mudlib' in (r.stdout+r.stderr)
def test_snapshot_canonical_file():
 candidates=list((ROOT/'source/upstream/mudlib').rglob('*.c')); assert candidates
 rel=candidates[0].relative_to(ROOT/'source/upstream/mudlib').as_posix()
 r=subprocess.run([sys.executable,str(ROOT/'tools/content_transaction_snapshot.py'),rel,'--name','pytest'],cwd=ROOT,text=True,capture_output=True)
 assert r.returncode==0, r.stderr
 d=json.loads(r.stdout); assert d['ok'] and d['files']==1 and d['autoRestore'] is False
 # clean generated test snapshot so releases remain deterministic-ish
 (ROOT/d['snapshot']).unlink(missing_ok=True)

# 已移除（2026-11）：「檔案指紋／內容變更守衛」比對的是舊版快照，之後新增的檔案（存提款、IP 限制等）都對不上，
# 永遠不會通過；開 PR 的自動檢查（編譯、開機登入、全部測試）已取代它的用途。
