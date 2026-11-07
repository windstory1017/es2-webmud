from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
bank=(ROOT/'source/upstream/mudlib/std/room/bank.c').read_text(encoding='utf-8')
bazar=(ROOT/'source/upstream/mudlib/d/snow/bazar.c').read_text(encoding='utf-8')

def test_bank_money_ids_cover_five_currencies():
    # 2026-11-07：錢莊只認 id（docs/CURRENCY.md）。
    for money_id in ['coin', 'silver', 'gold', 'sycee', 'nugget']:
        assert 'case "%s"' % money_id in bank

def test_deposit_resolves_real_money_objects_not_plain_id_collision():
    assert 'object find_player_money' in bank
    assert 'inherits(MONEY, ob)' in bank
    assert 'ob->query("money_id") == id' in bank
    assert 'find_player_money(this_player(), money)' in bank

def test_withdraw_uses_whitelist_money_factory():
    assert 'string money_file' in bank
    assert '({ "coin", "silver", "gold", "sycee", "nugget" })' in bank
    assert 'new(money_file(money))' in bank

def test_bank_sign_has_literal_examples():
    assert 'deposit 25 coin' in bazar
    assert 'withdraw 1 sycee' in bazar
    assert 'nugget（復刻紀念金元寶）' in bazar
