from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def table(src, name):
    body = src.split('private mapping %s = ([' % name, 1)[1].split(']);', 1)[0]
    return {k: dict((a, int(b)) for a, b in re.findall(r'"(\w+)": (-?\d+)', v))
            for k, v in re.findall(r'"([^"]+)": \(\[ (.*?) \]\),', body)}


def test_affix_tables_match_design():
    src = (MUD / 'adm/daemons/enhanced.c').read_text(encoding='utf-8')
    pre, suf = table(src, 'AFFIX_PREFIX'), table(src, 'AFFIX_SUFFIX')
    assert len(pre) == 36 and len(suf) == 9
    assert pre['黑鋼'] == {'damage': 15, 'attack': 10}
    assert pre['碧血'] == {'cps': 1, 'armor': 15}
    assert pre['修羅'] == {'cor': 2, 'intimidate': 15}
    assert pre['雛鐵'] == {'attack': 10, 'force': 10}
    assert suf['天鷹'] == {'awarness': 50} and suf['金鷹'] == {'wittiness': 30}
    # 必須有前綴才會有後綴；一般武器機率 80／15／5（倍數 1：r >= 100 - 20、r >= 100 - 5）。
    assert 'if( r >= 100 - 20 * chance_mult ) affix["prefix"]' in src
    assert 'if( r >= 100 - 5 * chance_mult ) affix["suffix"]' in src
    assert 'if( chance_mult < 1 ) chance_mult = 1;' in src
    assert 'if( !prefix || !stringp(suffix)' in src
    assert '#define AFFIX_COLOR 1' in src


def test_all_area_weapons_roll_on_creation():
    for f in ('longsword', 'shortsword', 'blade', 'glaive', 'bastardsword', 'broadsword', 'greatsword',
              'curved_blade', 'weirblade', 'broadaxe', 'pike', 'rod'):
        s = (MUD / 'obj/area/obj' / (f + '.c')).read_text(encoding='utf-8')
        assert 'if( clonep() ) ENHANCE_D->roll_affix(this_object());' in s, f
        assert 'enhance_weapon' not in s, f


def test_storage_chest_keeps_affix():
    s = (MUD / 'custom/home/obj/storage_chest.c').read_text(encoding='utf-8')
    assert 'data["affix"]' in s and 'ENHANCE_D->apply_affix(ob, data["affix"]);' in s


def test_bamboo_fishing_rod_keeps_base_trait():
    s = (MUD / 'custom/weapon/whip/bamboo_fishing_rod.c').read_text(encoding='utf-8')
    assert '"halieutics": 3,' in s and 'ENHANCE_D->roll_affix(this_object());' in s
    d = (MUD / 'adm/daemons/enhanced.c').read_text(encoding='utf-8')
    assert 'affix_base_apply/' in d   # 詞綴加在原本特性上，不會蓋掉
    assert 'bamboo_fishing_rod' in (MUD / 'd/wutang/npc/fisher.c').read_text(encoding='utf-8')


def test_affix_name_uses_weapon_kind_and_both_colors():
    src = (MUD / 'adm/daemons/enhanced.c').read_text(encoding='utf-8')
    short = dict(re.findall(r'"(\w+)": "([^"]+)",', src.split('private mapping AFFIX_BASE_SHORT = ([', 1)[1].split(']);', 1)[0]))
    assert short['sword'] == '劍' and short['blade'] == '刀' and short['dagger'] == '匕' and short['staff'] == '杖'
    # 只有前綴：整把用前綴的顏色；有後綴：前綴用前綴的顏色，後綴與武器名用後綴的顏色。
    assert 'pcolor = affix_color(prefix), scolor = affix_color(suffix)' in src
    assert '(pcolor ? pcolor + prefix + NOR : prefix)' in src
    assert '(scolor ? scolor + suffix + name + NOR : suffix + name)' in src
    assert 'name = affix_short_name(ob, name);' in src
    assert 'set("affix_short_name", "鞭");' in (MUD / 'obj/area/obj/rod.c').read_text(encoding='utf-8')
    assert 'set("affix_short_name", "竿");' in (MUD / 'custom/weapon/whip/bamboo_fishing_rod.c').read_text(encoding='utf-8')


def test_wutang_guard_wields_blade():
    s = (MUD / 'd/wutang/npc/guard.c').read_text(encoding='utf-8')
    # 刀法改由 NPC 自動技能依手上武器補上（docs/CUSTOM_NPC_SKILL.md），不再手寫。
    assert 'set_skill("blade"' not in s
    assert 'set_power("B");' in s
    assert 'carry_object("/obj/area/obj/blade")->wield();' in s


def test_dagger_and_staffs_roll_affix():
    for f, short in (('obj/area/obj/dagger.c', None), ('d/lee/obj/brow_staff.c', '棍'),
                     ('custom/weapon/staff/long_staff.c', None), ('custom/weapon/staff/zenstaff.c', '禪杖')):
        s = (MUD / f).read_text(encoding='utf-8')
        assert 'if( clonep() ) ENHANCE_D->roll_affix(this_object());' in s, f
        if short:
            assert 'set("affix_short_name", "%s");' % short in s, f


def test_vendor_sells_plain_weapons():
    # 商店賣的武器一律是普通品（使用者決定，2026-10-16）。
    s = (MUD / 'feature/npc/vendor.c').read_text(encoding='utf-8')
    assert 'ENHANCE_D->apply_affix(ob, ([]));' in s
    assert s.count('plain_goods(new(what))') == 3
    assert 'new(what)' not in s.replace('plain_goods(new(what))', '')


def test_colosus_sword_rolls_affix_three_times_as_often():
    # 巨劍（巨神劍）加入詞綴行列，出詞綴機會 3 倍：前綴 60%、前綴＋後綴 15%。
    s = (MUD / 'custom/weapon/sword/colosus_sword.c').read_text(encoding='utf-8')
    assert 'if( clonep() ) ENHANCE_D->roll_affix(this_object(), 3);' in s
    e = (MUD / 'adm/daemons/enhanced.c').read_text(encoding='utf-8')
    assert 'varargs void roll_affix(object ob, int chance_mult)' in e
    assert 'r >= 100 - 20 * chance_mult' in e and 'r >= 100 - 5 * chance_mult' in e
