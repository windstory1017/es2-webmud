from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'
CLOTH = MUD / 'custom' / 'armor' / 'cloth'


def read(p):
    return p.read_text(encoding='utf-8')


def test_every_cloth_is_worn_as_cloth():
    files = sorted(CLOTH.glob('*.c'))
    assert len(files) >= 63  # 之後的批次會再增加
    for f in files:
        s = read(f)
        assert 'inherit F_CLOTH;' in s, f.name
        assert 'set("wear_as", "cloth");' in s, f.name
        assert 'set("apply_armor/cloth", ([' in s, f.name
        w = int(re.search(r'set_weight\((\d+)\)', s).group(1))
        assert w in (800, 1000, 1200, 2000, 4000, 6000), f.name   # 1200：青色道袍（使用者設計表指定）
        v = int(re.search(r'set\("value", (\d+)\)', s).group(1))
        assert 0 < v <= 100000, f.name


def test_dark_cloth_thief_only():
    s = read(CLOTH / 'dark_cloth.c')
    assert 'owner->query_class() != "thief"' in s
    assert 'return ::wear(on_part);' in s


def test_cowhide_vest_renamed():
    assert 'set_name("牛皮背心"' in read(CLOTH / 'cowhide_vest.c')
    s = read(CLOTH / 'leather_vest.c')
    assert 'set_name("皮背心"' in s and '"armor": 2,' in s


def test_skill_names_and_spell_label():
    zh = read(MUD / 'data' / 'chinese.o')
    names = re.findall(r'^\| [^|]+ \| ([^|]+) \| `([^`]+)` \|', read(MUD.parents[2] / 'docs' / 'SKILL_NAMES.md'), re.M)
    assert len(names) == 19
    for v, k in names:
        assert f'"{k}":"{v.strip()}"' in zh, k
    for f, k in [('animitta_kasaya', 'compassion'), ('gold_robe', 'absorption'),
                 ('cloudy_silk_cloth', 'taoism of nature'), ('sky_earth_cloth', 'taoism of conviction'),
                 ('charm_robe', 'taoism of purify'), ('firewu_cloth', 'taoism-cloud')]:
        assert f'"{k}": ' in read(CLOTH / f'{f}.c'), f
    assert '"spell" : "咒文能力",' in read(MUD / 'cmds' / 'std' / 'identify.c')
    assert '"spell": 25,' in read(CLOTH / 'malik_robe.c')


def test_tight_cloth_follows_source_data():
    s = read(MUD / 'd' / 'lee' / 'obj' / 'tight_cloth.c')
    assert '"defense": 5,' in s and '"armor": 4,' in s


def test_no_retired_skill_codes():
    # 技能代碼改名後（docs/SKILL_NAMES.md），裝備與武器不能再用舊的暫定代碼
    old = ['"great-compassion"', '"dhyana-essence"', '"maoshan-illusion"', '"taoism-taoshan"']
    for d in ['armor', 'weapon']:
        for f in (MUD / 'custom' / d).rglob('*.c'):
            s = read(f)
            assert not any(o in s for o in old), f
