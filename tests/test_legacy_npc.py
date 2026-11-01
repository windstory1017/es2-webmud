from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
DIR = MUD / 'custom' / 'legacy_npc'
LIST = (ROOT / 'docs' / 'LEGACY_NPC_LIST.md').read_text(encoding='utf-8')
ROWS = re.findall(r'^\| (\d+) \| ([^|]+) \| ([^|]+) \| `([^`]+)` \| [^|]+ \| [^|]+ \| ([^|]+) \| (\d+) \| ([CBAS]) \| [^|]+ \| ([^|]+) \|$', LIST, re.M)
CLASS = {'武者': 'fighter', '軍人': 'soldier', '道士': 'taoist', '盜賊': 'thief', '平民': 'commoner',
         '書生': 'scholar', '方士': 'alchemist', '和尚': 'monk'}


def read(p):
    return p.read_text(encoding='utf-8')


def test_all_listed_npcs_exist():
    assert len(ROWS) == 126
    files = sorted(p.name for p in DIR.glob('*.c') if p.name != 'legacy_npc.c')
    assert len(files) == 126
    assert sorted(Path(r[3]).name for r in ROWS) == files


def test_npc_settings_follow_list():
    for no, name, eid, f, cls, lv, tier, beh in ROWS:
        s = read(MUD / f)
        assert 'inherit "/custom/legacy_npc/legacy_npc";' in s, f
        assert f'set_name("{name.strip()}", ({{ "{eid.strip().lower()}"' in s, f
        assert f'set_class("{CLASS[cls.strip()]}");' in s, f
        assert f'set_level({lv});' in s, f
        assert f'set_power("{tier}");' in s, f
        assert ('set("legacy_aggressive", 1);' in s) == ('主動攻擊' in beh), f
        assert ('set("legacy_patrol", 1);' in s) == ('巡邏' in beh), f
        assert 'set_skill(' not in s and 'set_attr(' not in s, f


def test_user_decisions():
    aggressive = {5, 6, 7, 8, 10, 16, 17, 19, 38, 41, 50, 51, 59, 60, 70, 125, 126}
    assert {int(r[0]) for r in ROWS if '主動攻擊' in r[7]} == aggressive
    assert {int(r[0]) for r in ROWS if '巡邏' in r[7]} == {15, 16, 17, 19, 80}
    assert '"si-ma lei"' in read(DIR / 'si_ma_lei.c')
    assert '"madam han"' in read(DIR / 'madam_han.c')
    assert 'set_class("commoner");' in read(DIR / 'yan_she.c')
    s = read(DIR / 'guard_of_hell.c')
    assert 'set_race("headless");' in s and 'set_class("fighter");' in s and 'set_level(30);' in s
    assert '/obj/area/obj/bastardsword' in read(DIR / 'lieutenant.c')
    assert '/obj/books/strange_tales_of_the_celestial_court' in read(DIR / 'o_yan_zhi.c')


def test_new_items():
    s = read(MUD / 'custom/armor/cloth/green_cloth.c')
    assert '"green cloth"' in s and '"armor": 3' in s and '"spi": 1' in s and '"int": 1' in s
    assert 'green_cloth' in read(DIR / 'yaksa.c')
    s = read(MUD / 'custom/armor/armor/heart_armor.c')
    assert '"heart armor"' in s and '"armor": 8' in s and '"cps": 1' in s
    assert 'heart_armor' in read(DIR / 'hsu_yen_chih.c')


def test_base_behaviour():
    s = read(DIR / 'legacy_npc.c')
    assert 'query("legacy_aggressive") && objectp(ob) && userp(ob)' in s
    assert '#define PATROL_INTERVAL 4' in s and '#define PATROL_CHANCE   10' in s
    assert 'room->query("map/area") == area' in s


def test_carried_items_exist():
    for f in DIR.glob('*.c'):
        for path in re.findall(r'carry_object\("([^"]+)"\)', read(f)):
            assert (MUD / (path.lstrip('/') + '.c')).exists(), (f.name, path)
