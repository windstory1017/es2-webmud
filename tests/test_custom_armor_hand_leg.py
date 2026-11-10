from pathlib import Path

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'
ARMOR = MUD / 'custom' / 'armor'


def read(p):
    return p.read_text(encoding='utf-8')


def test_counts_and_slots():
    for d, slot, feat, n in [('armor', 'armor', 'F_ARMOR', 39), ('hand', 'hand_eq', 'F_HAND_EQ', 21),
                             ('leg', 'leg_eq', 'F_LEG_EQ', 7)]:
        files = sorted((ARMOR / d).glob('*.c'))
        assert len(files) >= n, d  # 之後的批次會再增加
        for f in files:
            s = read(f)
            assert f'inherit {feat};' in s and f'set("wear_as", "{slot}");' in s, f.name
            assert f'set("apply_armor/{slot}", ([' in s, f.name


def test_unique_restricted_and_study():
    assert 'inherit F_UNIQUE;' in read(ARMOR / 'armor' / 'full_plate_of_fire_hawk.c')
    for p, c in [('armor/royal_armor', 'soldier'), ('hand/royal_gloves', 'soldier'), ('armor/dark_armor', 'thief'),
                 ('hand/dark_bracers', 'thief'), ('armor/soul_lapis_lazuli_plate', 'taoist')]:
        assert f'owner->query_class() != "{c}"' in read(ARMOR / f'{p}.c'), p
    s = read(ARMOR / 'armor' / 'black_harness.c')
    assert 'inherit F_STUDY;' in s and 'set("content", ([ "force": 100 ]));' in s
    assert 'set("required/skill", ([ "force": 1 ]));' in s   # 要先有基本內功（2026-11-03）
    assert '你無法理解玄晦戰甲上記載的內容。' in s


def test_new_skill_codes_and_zhenwu_items():
    assert '"throwing": 10,' in read(ARMOR / 'armor' / 'white_dragon_plate.c')
    assert '"celestial palm": 10,' in read(ARMOR / 'hand' / 'celestial_tigers_claw.c')
    g = read(MUD / 'custom' / 'zhenwu' / 'obj' / 'fire_gauntlets.c')
    assert '"armor": 5,' in g and '"armor_vs_fire": 15,' in g and '"twohanded axe": 10,' in g
    assert 'damage_vs_fire' not in g and 'set("value", 20000);' in g
    a = read(MUD / 'custom' / 'zhenwu' / 'obj' / 'steel_armor.c')
    assert '"damage": 5,' in a and '"cor": 2,' in a and '"armor": 25,' in a
