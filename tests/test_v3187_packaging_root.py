from pathlib import Path
R=Path(__file__).resolve().parents[1]

def test_runtime_identity_is_current_root_relative():
    v=(R/"VERSION").read_text(encoding="utf-8").strip()
    # 資料夾名稱現在就是 repo 名稱（es2-webmud），不再附版本號
    assert v.startswith("3.")
