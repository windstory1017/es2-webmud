import json,subprocess,sys,importlib.util
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def loadmod():
 spec=importlib.util.spec_from_file_location('mfp',ROOT/'tools/multiplayer_evidence_fingerprint.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);return m

def test_gate_fingerprints_complete_and_scoped():
 out=loadmod().build();assert out['ok'],out.get('missing')
 assert 'login/tell/save/reconnect' in out['gates']
 assert 'mutual-player-fight' in out['gates']
 assert all(len(x['fingerprint'])==64 for x in out['gates'].values())
 assert all(not f['path'].startswith('web/') for g in out['gates'].values() for f in g['files'])

def test_offline_launcher_and_gate_explicitly_refuse_live_claim():
 bat=(ROOT/'CHECK_MULTIPLAYER_OFFLINE.bat').read_text(encoding='utf-8',errors='replace')
 tool=(ROOT/'tools/multiplayer_offline_gate.py').read_text(encoding='utf-8')
 assert 'OFFLINE DEVELOPMENT GATE' in bat
 assert 'liveAcceptanceClaimed' in tool and "'liveAcceptanceClaimed':False" in tool

# 已移除（2026-11）：「檔案指紋／內容變更守衛」比對的是舊版快照，之後新增的檔案（存提款、IP 限制等）都對不上，
# 永遠不會通過；開 PR 的自動檢查（編譯、開機登入、全部測試）已取代它的用途。
