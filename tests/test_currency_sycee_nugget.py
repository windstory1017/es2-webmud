"""金元寶（sycee）與復刻紀念金元寶（nugget）：docs/CURRENCY.md（2026-11-07）。"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LIB = ROOT / "source/upstream/mudlib"


def read(rel):
    return (LIB / rel).read_text(encoding="utf-8")


def test_new_money_objects():
    sycee = read("obj/money/sycee.c")
    nugget = read("obj/money/nugget.c")
    assert 'set_name("金元寶", ({"sycee", "sycee_money"}))' in sycee
    assert 'set("base_value", 1000000 )' in sycee
    assert 'set("base_unit", "錠")' in sycee
    assert 'set_name("復刻紀念金元寶", ({"nugget", "nugget_money"}))' in nugget
    assert 'set("base_value", 100000000 )' in nugget
    assert 'set("base_unit", "錠")' in nugget
    assert "看起來外圓內方的扁平金屬。" in read("obj/money/coin.c")


def test_payment_breaks_small_to_large():
    s = read("feature/char/finance.c")
    block = s[s.index("pay_money(int amount"):]
    assert block.index('small < amount && sycee > 0') < block.index('small < amount && nugget > 0')
    assert 'set_small_money_value(small - amount)' in block


def test_display_stops_at_sycee():
    # 龍圖心經自 2026-11-09（#45）起不再顯示扣款金額，已無金額寫法，不在檢查之列。
    # 黃白之術自 2026-11-14 起成功訊息不顯示存入金額，同樣不在檢查之列。
    for rel in ["cmds/std/balance.c",
                "feature/npc/vendor.c", "feature/npc/currency.c", "obj/alchemist/pill_vendor.c",
                "std/room/hockshop.c"]:
        s = read(rel)
        assert "錠金元寶" in s, rel


def test_drunk_scatter_includes_new_money():
    s = read("daemon/condition/drunk.c")
    assert 'present("sycee_money", me)' in s
    assert 'present("nugget_money", me)' in s


def test_bazar_plaque_shown_verbatim():
    s = read("d/snow/bazar.c")
    assert "replace_program(BANK)" not in s.replace("不能再用 replace_program(BANK)", "")
    assert 'if( arg == "匾額" )' in s
    assert "\t" not in s[s.index("#define PLAQUE"):s.index("void create")]
    assert '"兌換﹕  convert <數量> <錢幣> to <另類錢幣>\\n"' in s


if __name__ == "__main__":
    for name, fn in list(globals().items()):
        if name.startswith("test_"):
            fn()
    print("ok")
