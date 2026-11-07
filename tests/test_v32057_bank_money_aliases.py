from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def read(rel): return (ROOT/rel).read_text(encoding="utf-8")

def test_bank_accepts_money_ids_only():
    # 2026-11-07 使用者指示：錢莊只認錢幣 id，不接受中文名稱（docs/CURRENCY.md）。
    s=read("source/upstream/mudlib/std/room/bank.c")
    block=s[s.index('string normalize_money_id'):s.index('object find_player_money')]
    for token in ['"coin"','"silver"','"gold"','"sycee"','"nugget"']:
        assert token in block
    for token in ['"文錢"','"銅錢"','"碎銀"','"黃金"','"金元寶"']:
        assert token not in block
    assert 'money = normalize_money_id(money);' in s
    assert 'from = normalize_money_id(from);' in s
    assert 'to = normalize_money_id(to);' in s

def test_bank_no_longer_requires_a_bond():
    s=read("source/upstream/mudlib/std/room/bank.c")
    assert 'if( !(bond = present("bankbond", this_player())) )' not in s
    assert '請你先開一個戶頭' not in s

def test_combat_hud_uses_ten_segments():
    s=read("web/app.js")
    block=s[s.index("function combatStat"):s.index("function combatCard")]
    assert "const segments=10" in block
    assert "25" not in block
