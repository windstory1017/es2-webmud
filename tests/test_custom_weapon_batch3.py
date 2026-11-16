from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'
W = MUD / 'custom' / 'weapon'


def read(p):
    return p.read_text(encoding='utf-8')


def test_counts_and_features():
    for d, feat, n in [('whip', 'F_WHIP', 10), ('needle', 'F_NEEDLE', 7), ('blunt', 'F_BLUNT', 7),
                       ('pike', 'F_PIKE', 19), ('sword', 'F_SWORD', 56), ('dagger', 'F_DAGGER', 22)]:
        files = sorted((W / d).glob('*.c'))
        assert len(files) >= n, d  # 之後的批次會再增加
        for f in files:
            s = read(f)
            assert f'inherit {feat};' in s and 'init_damage(' in s and 'set("wield_as"' in s, f.name
            # 名稱可能是字串，也可能是 HIC "逐風刃" NOR 這種顏色巨集寫法
            name = re.search(r'set_name\((.*?),\s*\(\{', s, re.S).group(1)
            name = name.strip().strip('"')
            assert '\\x1b' not in name or name.endswith('\\x1b[m') or name.endswith('\\x1b[0m'), f.name


def test_needles_light():
    for f in (W / 'needle').glob('*.c'):
        assert 'set_weight(500);' in read(f), f.name


def test_unique():
    for p in ['whip/dragon_whip', 'needle/needle_of_bone_maggot', 'pike/solar_pike', 'pike/wraith_of_warlord',
              'sword/sword_of_killing_god', 'sword/sword_of_mighty_dragon', 'sword/yiyoung_sword', 'sword/sword_of_raven',
              'sword/numinous_sword', 'sword/sword_of_wind_spring', 'sword/au_han_sword']:
        assert 'inherit F_UNIQUE;' in read(W / f'{p}.c'), p


def test_poisons():
    base = read(MUD / 'custom' / 'condition' / 'weapon_poison.c')
    assert '你中的毒發作了！' in base and 'data["left"] = burst_count();' in base
    # 先扣目前值、再扣實格（500/500 -> 490/492）
    assert base.index('me->consume_stat(d[0], d[2], from);') < base.index('me->damage_stat(d[0], d[1], from);')
    # 使用者數值 A/B：A = 目前值扣、B = 最大值扣；檔案裡是 ({ 屬性, 最大值, 目前值 })
    for w, c, t, n, dm in [('blunt/skull_heart', 'skull_heart_poison', 2, 4, [('gin', 5, 3), ('kee', 8, 5), ('sen', 2, 1)]),
                           ('dagger/wicked_dagger', 'rain_poison', 2, 4, [('gin', 8, 4), ('kee', 8, 4), ('sen', 2, 1)]),
                           ('sword/hundreds_poison_sword', 'hundred_poison', 3, 3, [('gin', 7, 3), ('kee', 10, 5), ('sen', 2, 1)]),
                           ('pike/wraith_of_warlord', 'wraith_poison', 2, 5, [('gin', 4, 2), ('kee', 5, 3)]),
                           ('pike/gin_pike', 'phoenix_poison', 2, 5, [('gin', 4, 2), ('kee', 5, 3)]),
                           ('needle/needle_of_bone_maggot', 'maggot_poison', 2, 5, [('gin', 10, 8), ('kee', 8, 5), ('sen', 4, 3)])]:
        assert f'CONDITION_D("{c}")->poison(victim, me);' in read(W / f'{w}.c'), w
        s = read(MUD / 'daemon' / 'condition' / f'{c}.c')
        assert f'int burst_ticks() {{ return {t}; }}' in s and f'int burst_count() {{ return {n}; }}' in s, c
        for st, cur, mx in dm:
            assert f'({{ "{st}", {mx}, {cur} }})' in s, (c, st)


def test_special_cases():
    s = read(W / 'sword' / 'heaven_sword.c')
    assert '"sword");' in s and '"secondhand sword");' in s
    assert 'apply_weapon/secondhand sword' in s and 'apply_weapon/sword"' not in s
    assert 'init_damage(2, 5, 50, 0, "secondhand sword");' in read(W / 'sword' / 'black_shortsword.c')
    assert 'set_name("\\x1b[1;37m洗銀劍\\x1b[m"' in read(W / 'sword' / 'silver_sword.c')
    assert (W / 'dagger' / 'dagger_of_frost_edge.c').exists()
