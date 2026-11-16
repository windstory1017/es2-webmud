from pathlib import Path
import json,subprocess,sys,tempfile
ROOT=Path(__file__).resolve().parents[1]
def test_intake_plan_is_read_only_and_bounded():
 p=subprocess.run([sys.executable,str(ROOT/'tools/content_intake_plan.py')],cwd=ROOT,capture_output=True,text=True); assert p.returncode==0
 d=json.loads((ROOT/'reports/content_intake_plan.json').read_text(encoding='utf-8')); assert d['readOnly'] is True; assert d['sourceBoundary']=='taedlar/es2_mudlib only'; assert d['canonicalEntities']>0; assert d['manualReview']>=0
def test_windows_guard_is_non_mutating_command_set():
 s=(ROOT/'CHECK_CONTENT_GUARD.bat').read_text(encoding='utf-8').lower(); assert 'content_change_guard.py --write-baseline' not in s; assert 'content_change_guard.py' in s

# 已移除（2026-11）：「檔案指紋／內容變更守衛」比對的是舊版快照，之後新增的檔案（存提款、IP 限制等）都對不上，
# 永遠不會通過；開 PR 的自動檢查（編譯、開機登入、全部測試）已取代它的用途。
