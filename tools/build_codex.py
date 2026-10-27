#!/usr/bin/env python3
"""產生 ES2 WebMUD 內容清單網頁：技能、武器、防具、物品、區域。

直接掃描 mudlib 原始碼（d/、obj/、custom/、daemon/skill/），整理成資料後嵌進
tools/codex_template.html，輸出一個不依賴其他檔案的網頁。

用法：
    python3 tools/build_codex.py                  # 輸出 docs/catalog/es2_catalog.html
    python3 tools/build_codex.py 其他輸出路徑.html
"""
import json
import re
import sys
from collections import defaultdict, deque
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
TEMPLATE = ROOT / 'tools' / 'codex_template.html'
OUT = ROOT / 'docs' / 'catalog' / 'es2_catalog.html'
SCAN_DIRS = ['d', 'obj', 'custom']

# --------------------------------------------------------------------------- 名稱對照

NAMED = {  # 與 cmds/std/identify.c 相同
    'attack': '攻擊能力值', 'defense': '防禦能力值', 'str': '膂力', 'cor': '膽識', 'cps': '定力',
    'int': '悟性', 'wis': '慧根', 'spi': '靈性', 'con': '根骨', 'dex': '機敏', 'kar': '福緣',
    'damage': '傷害力', 'armor': '防禦力', 'spell': '咒文能力', 'magic_ability': '魔力',
    'gin': '精', 'kee': '氣', 'sen': '神', 'HP': '形體', 'food': '食物', 'water': '飲水',
}
SLOT = {'armor': '護甲', 'cloth': '衣服', 'head_eq': '頭部', 'neck_eq': '項鍊', 'finger_eq': '戒指',
        'waist_eq': '腰帶', 'hand_eq': '手部', 'leg_eq': '腿部', 'feet_eq': '鞋子', 'wrist_eq': '手腕'}
SLOT_ORDER = list(SLOT)
WEAPON_KIND = {'blade': '刀', 'sword': '劍', 'staff': '杖', 'axe': '斧', 'dagger': '匕首', 'pike': '槍',
               'whip': '鞭', 'blunt': '錘棒', 'needle': '針', 'throwing': '暗器'}
KIND_ORDER = list(WEAPON_KIND)
CLASS_NAME = {'soldier': '軍人', 'fighter': '武者', 'thief': '盜賊', 'taoist': '道士', 'alchemist': '方士',
              'scholar': '書生', 'monk': '和尚', 'commoner': '平民'}
RACE_NAME = {'headless': '形天族', 'human': '人類', 'malik': '巫首', 'yaksa': '夜叉', 'ashura': '阿修羅'}
DIR_AREA = {  # 沒有 map/area 的房間，依目錄給區域名稱
}
# 不公開的目錄：這些目錄的房間不列入區域；裡面的 NPC 與物品只有被公開房間放置或
# 公開 NPC 攜帶時才列出（例如家園傳送師放在雪亭鎮客棧）。
HIDDEN_DIRS = ['/custom/wizroom/', '/custom/home/']
# 地圖手動調整（只影響圖鑑顯示）：房間 -> (參考房間, 往東格數, 往南格數)
MAP_POS = {
    # 百花村（d/baihua）：依設計表格位（相對於山腰上 F10）。懸崖平台靠 climb／eastup 進出、
    # 荒野小徑靠 search 進入，自動排版會放錯，全部手動指定。
    '/d/baihua/platform': ('/d/baihua/hillside', 1, -1),
    '/d/baihua/cliff': ('/d/baihua/hillside', 1, 0),
    '/d/baihua/tile_house': ('/d/baihua/hillside', 2, 0),
    '/d/baihua/stone_road1': ('/d/baihua/hillside', 3, 0),
    '/d/baihua/cliff_edge': ('/d/baihua/hillside', 4, 0),
    '/d/baihua/school': ('/d/baihua/hillside', 2, 1),
    '/d/baihua/stone_road2': ('/d/baihua/hillside', 3, 1),
    '/d/baihua/wang_home': ('/d/baihua/hillside', 4, 1),
    '/d/baihua/trail': ('/d/baihua/hillside', 2, 2),
    '/d/baihua/road1': ('/d/baihua/hillside', 3, 2),
    '/d/baihua/alley': ('/d/baihua/hillside', 4, 2),
    '/d/baihua/road2': ('/d/baihua/hillside', 3, 3),
    '/d/baihua/inn': ('/d/baihua/hillside', 4, 3),
    '/d/baihua/temple': ('/d/baihua/hillside', 2, 4),
    # 五堂鎮：依設計表格子座標（相對於五堂鎮口 N8）；第四個數字是樓層（客棧二、三樓）。
    # 老松林／迷霧森林（d/oldpine）：依設計表格位，以各區入口為原點。
    '/d/oldpine/clearing_n': ('/d/oldpine/entrance', 8, 1),
    '/d/oldpine/forest_n1': ('/d/oldpine/entrance', 5, 1),
    '/d/oldpine/forest_n2': ('/d/oldpine/entrance', 6, 1),
    '/d/oldpine/forest_n3': ('/d/oldpine/entrance', 7, 1),
    '/d/oldpine/clearing_w': ('/d/oldpine/entrance', 0, 1),
    '/d/oldpine/path0': ('/d/oldpine/entrance', 1, 2),
    '/d/oldpine/path1': ('/d/oldpine/entrance', 2, 2),
    '/d/oldpine/forest_w': ('/d/oldpine/entrance', 4, 2),
    '/d/oldpine/grass1': ('/d/oldpine/entrance', 8, 2),
    '/d/oldpine/bear_cave': ('/d/oldpine/entrance', 9, 2),
    '/d/oldpine/path2': ('/d/oldpine/entrance', 3, 3),
    '/d/oldpine/crossing': ('/d/oldpine/entrance', 4, 3),
    '/d/oldpine/grass2': ('/d/oldpine/entrance', 9, 3),
    '/d/oldpine/hideout': ('/d/oldpine/entrance', 3, 4),
    '/d/oldpine/side_hall_w': ('/d/oldpine/entrance', 0, 4),
    '/d/oldpine/main_hall': ('/d/oldpine/entrance', 1, 4),
    '/d/oldpine/side_hall_e': ('/d/oldpine/entrance', 2, 4),
    '/d/oldpine/temple_square': ('/d/oldpine/entrance', 1, 5),
    '/d/oldpine/temple_gate': ('/d/oldpine/entrance', 2, 5),
    '/d/oldpine/gravel_road': ('/d/oldpine/entrance', 3, 5),
    '/d/oldpine/highway': ('/d/oldpine/entrance', 3, 6),
    '/d/oldpine/mist1': ('/d/oldpine/mist_entrance', 1, 0),
    '/d/oldpine/mist2': ('/d/oldpine/mist_entrance', 2, 0),
    '/d/oldpine/mist3': ('/d/oldpine/mist_entrance', 2, 1),
    '/d/oldpine/mist4': ('/d/oldpine/mist_entrance', 3, 1),
    '/d/oldpine/kitchen': ('/d/oldpine/mist_entrance', 1, 2),
    '/d/oldpine/inn': ('/d/oldpine/mist_entrance', 2, 2),
    '/d/oldpine/glade': ('/d/oldpine/mist_entrance', 3, 2),
    '/d/oldpine/wood1': ('/d/oldpine/mist_entrance', 4, 2),
    '/d/oldpine/beast_trail': ('/d/oldpine/mist_entrance', 5, 1),
    '/d/oldpine/cave_mouth': ('/d/oldpine/mist_entrance', 6, 1),
    '/d/oldpine/cave1': ('/d/oldpine/mist_entrance', 7, 1),
    '/d/oldpine/cave2': ('/d/oldpine/mist_entrance', 8, 1),
    '/d/oldpine/cave3': ('/d/oldpine/mist_entrance', 7, 2),
    '/d/oldpine/cave_deep': ('/d/oldpine/mist_entrance', 8, 2),
    '/d/oldpine/ledge': ('/d/oldpine/mist_entrance', 8, 3),
    '/d/oldpine/shrubs': ('/d/oldpine/mist_entrance', 3, 3),
    '/d/oldpine/wood2': ('/d/oldpine/mist_entrance', 4, 3),
    '/d/oldpine/bandit_clearing': ('/d/oldpine/mist_entrance', 2, 4),
    '/d/oldpine/reeds': ('/d/oldpine/mist_entrance', 3, 4, -1),
    '/d/oldpine/reeds_exit': ('/d/oldpine/mist_entrance', 3, 4, 1),
    '/d/oldpine/wood3': ('/d/oldpine/mist_entrance', 4, 4),
    '/d/oldpine/wood4': ('/d/oldpine/mist_entrance', 3, 5),
    '/d/oldpine/wood_deep': ('/d/oldpine/mist_entrance', 3, 6),
    '/d/wutang/inn2': ('/d/wutang/inn', 0, 0, 1),
    '/d/wutang/inn3': ('/d/wutang/inn', 0, 0, 2),
    '/d/wutang/temple': ('/d/wutang/entrance', 5, -2),
    '/d/wutang/temple_yard': ('/d/wutang/entrance', 6, -2),
    '/d/wutang/temple_road_n': ('/d/wutang/entrance', 5, -1),
    '/d/wutang/path_e': ('/d/wutang/entrance', 6, -1),
    '/d/wutang/backyard': ('/d/wutang/entrance', 6, 0),
    '/d/wutang/temple_road_s': ('/d/wutang/entrance', 5, 0),
    '/d/wutang/market_square': ('/d/wutang/entrance', 5, 1),
    '/d/wutang/yan_hall': ('/d/wutang/entrance', 4, -2),
    '/d/wutang/yan_mansion': ('/d/wutang/entrance', 4, -1),
    '/d/wutang/yan_gate': ('/d/wutang/entrance', 4, 0),
    '/d/wutang/dark_alley_e': ('/d/wutang/entrance', 3, 0),
    '/d/wutang/dark_alley_w': ('/d/wutang/entrance', 2, 0),
    '/d/wutang/school': ('/d/wutang/entrance', -2, 1),
    '/d/wutang/pawnshop': ('/d/wutang/entrance', -1, 1),
    '/d/wutang/north_street': ('/d/wutang/entrance', 0, 1),
    '/d/wutang/inn': ('/d/wutang/entrance', 2, 1),
    '/d/wutang/cloth_shop': ('/d/wutang/entrance', 3, 1),
    '/d/wutang/pavilion': ('/d/wutang/entrance', -4, 2),
    '/d/wutang/gravel_road_n': ('/d/wutang/entrance', -3, 2),
    '/d/wutang/west_street2': ('/d/wutang/entrance', -2, 2),
    '/d/wutang/west_street1': ('/d/wutang/entrance', -1, 2),
    '/d/wutang/crossroad': ('/d/wutang/entrance', 0, 2),
    '/d/wutang/east_street1': ('/d/wutang/entrance', 1, 2),
    '/d/wutang/east_street2': ('/d/wutang/entrance', 2, 2),
    '/d/wutang/east_street3': ('/d/wutang/entrance', 3, 2),
    '/d/wutang/east_street4': ('/d/wutang/entrance', 4, 2),
    '/d/wutang/city_god_temple': ('/d/wutang/entrance', 5, 2),
    '/d/wutang/gravel_road_s': ('/d/wutang/entrance', -4, 3),
    '/d/wutang/bamboo_hall': ('/d/wutang/entrance', -3, 3),
    '/d/wutang/riverside': ('/d/wutang/entrance', -5, 3),
    '/d/wutang/south_street1': ('/d/wutang/entrance', 0, 3),
    '/d/wutang/bank': ('/d/wutang/entrance', 1, 3),
    '/d/wutang/guesthouse': ('/d/wutang/entrance', 3, 3),
    '/d/wutang/boat': ('/d/wutang/entrance', -5, 4),
    '/d/wutang/bamboo_grove': ('/d/wutang/entrance', -3, 4),
    '/d/wutang/south_street2': ('/d/wutang/entrance', 0, 4),
    '/d/wutang/three_way': ('/d/wutang/entrance', 0, 5),
    '/d/wutang/grass_nw': ('/d/wutang/entrance', -2, 3),
    '/d/wutang/grass_ne': ('/d/wutang/entrance', -1, 3),
    '/d/wutang/grass_sw': ('/d/wutang/entrance', -2, 4),
    '/d/wutang/grass_se': ('/d/wutang/entrance', -1, 4),
    '/d/wutang/ferry': ('/d/wutang/entrance', -3, 5),
    '/d/wutang/boardwalk_w': ('/d/wutang/entrance', -2, 5),
    '/d/wutang/boardwalk_e': ('/d/wutang/entrance', -1, 5),
    '/d/wutang/ferry_dock': ('/d/wutang/entrance', -3, 6),
    '/d/wutang/river_bank': ('/d/wutang/entrance', -2, 7),
    '/d/wutang/hut': ('/d/wutang/entrance', -6, -1),
    '/d/wutang/field1': ('/d/wutang/entrance', -5, -1),
    '/d/wutang/field2': ('/d/wutang/entrance', -5, 0),
    '/d/wutang/field3': ('/d/wutang/entrance', -4, 0),
    '/d/wutang/field4': ('/d/wutang/entrance', -5, 1),
    '/d/wutang/field5': ('/d/wutang/entrance', -4, 1),
    '/d/wutang/clearing': ('/d/wutang/entrance', -3, 1),
    '/d/snow/fireplace': ('/d/snow/inn_kitchen', 0, -1),   # 大灶：客棧廚房上方
    '/d/snow/tree': ('/d/snow/square', 1, -1),             # 榕樹上：廣場中央右上
    '/d/snow/mill': ('/d/snow/ruin1', 0, -1),              # 磨坊：破舊大宅上方
    '/d/snow/riverbank': ('/d/snow/egate', 0, -2),         # 河邊空地：河邊上方第二格
    '/d/snow/ruin2': ('/d/snow/riverbank', 0, 1),          # 破舊大宅正廳：河邊空地下方（緊貼河邊上方）
    '/d/snow/kitchen': ('/d/snow/epath', 0, -1),           # 有錢人家廚房：僻靜小巷上方（climb 圍牆）
}
MAP_HIDE = {'/d/snow/inn_staff_room', '/d/oldpine/reeds2', '/d/oldpine/reeds3', '/d/oldpine/reeds4', '/d/oldpine/reeds5'}  # 不在地圖上顯示的房間

DIRS = {  # 地圖座標：x 往東、y 往南、z 往上
    'north': (0, -1, 0), 'south': (0, 1, 0), 'east': (1, 0, 0), 'west': (-1, 0, 0),
    'northeast': (1, -1, 0), 'northwest': (-1, -1, 0), 'southeast': (1, 1, 0), 'southwest': (-1, 1, 0),
    'up': (0, 0, 1), 'down': (0, 0, -1),
}


def load_chinese():
    try:
        s = (MUD / 'data' / 'chinese.o').read_text(encoding='utf-8', errors='replace')
    except OSError:
        return {}
    return dict(re.findall(r'"([^"]*)":"([^"]*)"', s))


CHINESE = load_chinese()


def zh(key):
    return NAMED.get(key) or CHINESE.get(key) or key


# --------------------------------------------------------------------------- LPC 解析小工具

def read(p):
    return p.read_text(encoding='utf-8', errors='replace')


def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', '', s, flags=re.S)
    s = re.sub(r'(?m)^\s*//.*$', '', s)
    # 有些檔案寫成 set_name ("x", ...)：函式名稱與括號之間的空白統一拿掉
    return re.sub(r'\b(set\w*|carry_object|init_damage|setup_\w+|advance_stat|map_skill)\s+\(', r'\1(', s)


def mud_path(p):
    return '/' + str(p.relative_to(MUD).with_suffix('')).replace('\\', '/')


def resolve(expr, here):
    """把 __DIR__"x"、"/d/x"、STOCK_WEAPON("x") 之類的路徑運算式轉成 /d/x。"""
    expr = expr.strip()
    base = here.rsplit('/', 1)[0] + '/'
    m = re.match(r'STOCK_(?:WEAPON|ARMOR)\(\s*"([^"]+)"\s*\)', expr)
    if m:
        return '/obj/area/obj/' + m.group(1)
    parts = re.findall(r'__DIR__|"((?:[^"\\]|\\.)*)"', expr)
    if not parts:
        return None
    out = ''
    for tok in re.finditer(r'__DIR__|"((?:[^"\\]|\\.)*)"', expr):
        out += base if tok.group(0) == '__DIR__' else tok.group(1)
    if not out.startswith('/'):
        out = base + out
    out = re.sub(r'\.c$', '', out)
    return re.sub(r'/+', '/', out)


ANSI_MACRO = {'NOR': '0', 'BLK': '30', 'RED': '31', 'GRN': '32', 'YEL': '33', 'BLU': '34', 'MAG': '35', 'CYN': '36',
              'WHT': '37', 'HIK': '1;30', 'HIR': '1;31', 'HIG': '1;32', 'HIY': '1;33', 'HIB': '1;34', 'HIM': '1;35',
              'HIC': '1;36', 'HIW': '1;37'}


def lpc_string(expr):
    """把 "a" "b"、HIC "a" NOR 或 @LONG...LONG 這類字串運算式合併成文字。"""
    m = re.match(r'\s*@(\w+)\n(.*?)\n\1', expr, re.S)
    if m:
        return m.group(2) + '\n'
    parts = []
    for tok in re.finditer(r'"((?:[^"\\]|\\.)*)"|\b([A-Z]{3})\b', expr):
        if tok.group(1) is not None:
            parts.append(tok.group(1))
        elif tok.group(2) in ANSI_MACRO:
            parts.append('\x1b[%sm' % ANSI_MACRO[tok.group(2)])
    s = ''.join(parts)
    s = s.replace('\\n', '\n').replace('\\"', '"').replace('\\x1b', '\x1b').replace('\\t', '  ')
    return s


def set_value(src, key):
    """取 set("key", ...) 的原始運算式文字。"""
    m = re.search(r'\bset\s*\(\s*"%s"\s*,' % re.escape(key), src)
    if not m:
        return None
    i, depth, j = m.end(), 1, m.end()
    in_str = False
    while j < len(src):
        c = src[j]
        if in_str:
            if c == '\\':
                j += 2
                continue
            if c == '"':
                in_str = False
        elif c == '"':
            in_str = True
        elif c == '@' and re.match(r'@(\w+)\n', src[j:]):
            tag = re.match(r'@(\w+)\n', src[j:]).group(1)
            k = src.find('\n' + tag, j + 1)
            j = k + len(tag) + 1 if k > 0 else j + 1
            continue
        elif c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
            if depth == 0:
                return src[i:j]
        j += 1
    return None


def set_int(src, key):
    v = set_value(src, key)
    if v is None:
        return None
    m = re.match(r'\s*(-?\d+)\s*$', v)
    return int(m.group(1)) if m else None


def set_str(src, key):
    v = set_value(src, key)
    return lpc_string(v) if v is not None else None


def mapping_pairs(expr):
    return [(k, int(v)) for k, v in re.findall(r'"([^"]+)"\s*:\s*(-?\d+)', expr or '')]


def path_mapping(expr, here):
    out = {}
    for m in re.finditer(r'((?:__DIR__\s*)?"[^"]*"|STOCK_\w+\(\s*"[^"]+"\s*\))\s*:\s*([^,\]]+)', expr or ''):
        p = resolve(m.group(1), here)
        if p is None:
            continue
        v = m.group(2).strip()
        out[p] = int(v) if re.fullmatch(r'-?\d+', v) else v
    return out


def name_ids(src):
    m = re.search(r'set_name\(\s*((?:[A-Z]{3}\s*\+?\s*)*"(?:[^"\\]|\\.)*"(?:\s*\+?\s*[A-Z]{3})*)\s*,\s*(\(\{.*?\}\)|"[^"]*")', src, re.S)
    if not m:
        m2 = re.search(r'set_name\(\s*((?:\w+\s*\+\s*)?"(?:[^"\\]|\\.)*"(?:\s*\+\s*\w+)?)', src)
        if not m2:
            return None, []
        return lpc_string(m2.group(1)), []
    return lpc_string(m.group(1)), re.findall(r'"([^"]*)"', m.group(2))


def money_text(copper):
    if copper is None:
        return None
    if copper >= 100:
        t = copper // 100
        r = copper % 100
        return f'{t} 兩' + (f' {r} 文' if r else '')
    return f'{copper} 文'


# --------------------------------------------------------------------------- 武器傷害

SETUP = {  # feature/weapon/*.c 的 setup_*()：(x, y, z, r) -> [(技能, x, y, z, r)]
    'blade': lambda x, y, z, r: [('blade', x, y, z, r), ('secondhand blade', x, y // 2, z, r), ('twohanded blade', x, y * 4 // 3, z * 3 // 2, r)],
    'sword': lambda x, y, z, r: [('sword', x, y, z, r), ('secondhand sword', x, y // 2, z, r), ('twohanded sword', x, y * 3 // 2, z * 4 // 3, r)],
    'staff': lambda x, y, z, r: [('staff', x, y, z, r), ('secondhand staff', x, y, z // 2, r), ('twohanded staff', x + 1, y * 3 // 2, z * 3 // 2, r)],
    'axe': lambda x, y, z, r: [('axe', x, y, z, r), ('secondhand axe', x, y // 2, z, r), ('twohanded axe', x, y * 2, z * 4 // 3, r)],
    'dagger': lambda x, y, z, r: [('dagger', x, y, z, r), ('secondhand dagger', x, y, z, r)],
    'pike': lambda x, y, z, r: [('pike', x, y, z, r), ('secondhand pike', x, y, z // 2, r), ('twohanded pike', x + 1, y * 4 // 3, z, r)],
    'blunt': lambda x, y, z, r: [('blunt', x, y, z, r), ('secondhand blunt', x, y, z * 2 // 3, r), ('twohanded blunt', x + 1, y, z * 2, r)],
    'whip': lambda x, y, z, r: [('whip', x, y, z, r)],
    'needle': lambda x, y, z, r: [('needle', x, y, z, r), ('secondhand needle', x, y, z, r)],
}


def weapon_damage(src):
    dmg = {}
    for m in re.finditer(r'setup_(\w+)\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\)', src):
        kind = m.group(1)
        if kind in SETUP:
            for sk, x, y, z, r in SETUP[kind](*map(int, m.groups()[1:])):
                dmg[sk] = (x, y, z, r)
    for m in re.finditer(r'init_damage\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*([^;]*?)\)\s*;', src):
        for sk in re.findall(r'"([^"]+)"', m.group(5)):
            dmg[sk] = tuple(map(int, m.groups()[:4]))
    return dmg


def weapon_kind(src, wield):
    m = re.search(r'inherit\s+F_(SWORD|BLADE|BLUNT|AXE|DAGGER|NEEDLE|STAFF|PIKE|WHIP)\b', src)
    if m:
        return m.group(1).lower()
    for sk in wield:
        base = sk.split()[-1]
        if base in WEAPON_KIND:
            return base
    m = re.search(r'setup_(\w+)\(', src)
    if m and m.group(1) in WEAPON_KIND:
        return m.group(1)
    m = re.search(r'inherit\s+(SWORD|BLADE|THROWING)\b', src)
    if m:
        return m.group(1).lower()
    return 'blade'


# --------------------------------------------------------------------------- 共同屬性

def common(p, src, code):
    name, ids = name_ids(code)
    item = {
        'id': mud_path(p),
        'file': str(p.relative_to(ROOT)).replace('\\', '/'),
        'name': name or p.stem,
        'ids': ids,
        'long': (set_str(code, 'long') or '').rstrip('\n'),
    }
    w = re.search(r'set_weight\(\s*(\d+)\s*\)', code)
    item['weight'] = int(w.group(1)) if w else (set_int(code, 'base_weight') or set_int(code, 'weight'))
    v = set_int(code, 'value')
    if v is None:
        v = set_int(code, 'base_value')
    item['value'] = money_text(v) if v is not None else None
    item['unit'] = set_str(code, 'unit')
    flags = []
    if 'F_UNIQUE' in code:
        flags.append('唯一')
    poison = re.search(r'CONDITION_D\("([^"]+)"\)->poison|/daemon/condition/(\w+)"\s*->\s*poison', code)
    if poison:
        flags.append('劇毒')
        item['poison'] = poison.group(1) or poison.group(2)
    if 'miss_ob' in code:
        flags.append('特攻')
    for c in re.findall(r'query_class\(\)\s*!=\s*"(\w+)"', code):
        flags.append('限' + CLASS_NAME.get(c, c))
    for r in re.findall(r'query_race\(\)\s*!=\s*"(\w+)"', code):
        flags.append('限' + RACE_NAME.get(r, r))
    if 'F_STUDY' in code:
        flags.append('可研讀')
    verbs = re.findall(r'add_action\(\s*"\w+"\s*,\s*"([\w ]+)"\s*\)', code)
    if verbs:
        item['verbs'] = verbs
    item['flags'] = flags
    return item


def traits(code, prefix):
    out = {}
    for m in re.finditer(r'\bset\s*\(\s*"%s/([^"]+)"\s*,\s*\(\[(.*?)\]\)\s*\)' % prefix, code, re.S):
        out[m.group(1)] = [[zh(k), v, k] for k, v in mapping_pairs(m.group(2))]
    for m in re.finditer(r'\bset\s*\(\s*"%s/([^"/]+)/([^"]+)"\s*,\s*(-?\d+)\s*\)' % prefix, code):
        out.setdefault(m.group(1), []).append([zh(m.group(2)), int(m.group(3)), m.group(2)])
    return out


# --------------------------------------------------------------------------- 各類物件

ROOM_INHERIT = re.compile(r'inherit\s+(ROOM|BANK|HOCKSHOP|INN|TEMPLE|"/std/room[^"]*")')
NPC_INHERIT = re.compile(r'inherit\s+(NPC|F_VILLAGER|F_VENDOR|F_FIGHTER|F_SOLDIER|F_BANDIT|F_TRAINER|F_TEACHER|"/custom/ghost/std_ghost"|"/std/char/npc")')
WEAPON_INHERIT = re.compile(r'inherit\s+(F_(SWORD|BLADE|BLUNT|AXE|DAGGER|NEEDLE|STAFF|PIKE|WHIP)|SWORD|BLADE|THROWING)\b')
ARMOR_INHERIT = re.compile(r'inherit\s+(F_(ARMOR|CLOTH|HEAD_EQ|HAND_EQ|FEET_EQ|NECK_EQ|FINGER_EQ|WAIST_EQ|LEG_EQ|WRIST_EQ)|CLOTH|EQUIP)\b')


def classify(code):
    if ROOM_INHERIT.search(code) or (re.search(r'\bset\s*\(\s*"exits"', code) and re.search(r'\bset\s*\(\s*"short"', code) and 'set_name' not in code):
        return 'room'
    if NPC_INHERIT.search(code) or re.search(r'set_race\(', code):
        return 'npc'
    if WEAPON_INHERIT.search(code) or re.search(r'init_damage\(|setup_(sword|blade|blunt|axe|dagger|needle|staff|pike|whip)\(', code):
        return 'weapon'
    if ARMOR_INHERIT.search(code) or set_value(code, 'wear_as') is not None:
        return 'armor'
    if 'set_name' in code:
        return 'item'
    return None


def parse_weapon(p, src, code):
    it = common(p, src, code)
    wv = set_value(code, 'wield_as')
    wield = re.findall(r'"([^"]+)"', wv) if wv else []
    dmg = weapon_damage(code)
    if not wield:
        wield = list(dmg)
    it['kind'] = weapon_kind(code, wield)
    rows = []
    for sk in wield:
        d = dmg.get(sk)
        row = {'skill': zh(sk), 'code': sk}
        if d:
            x, y, z, r = d
            row['dmg'] = f'{r + x} - {r + x * y}'
            row['bonus'] = f'{z}%'
            row['mult'] = x
        rows.append(row)
    it['usages'] = rows
    it['traits'] = {zh(k): v for k, v in traits(code, 'apply_weapon').items()}
    if not dmg:
        m = re.search(r'init_(blade|sword|throwing)\(\s*(\d+)\s*\)', code)
        if m:
            it['usages'] = [{'skill': zh(m.group(1)), 'code': m.group(1), 'dmg': f'基礎 {m.group(2)}'}]
    return it


def parse_armor(p, src, code):
    it = common(p, src, code)
    wv = set_value(code, 'wear_as')
    slots = re.findall(r'"([^"]+)"', wv) if wv else []
    tr = traits(code, 'apply_armor')
    if not slots:
        slots = list(tr)
    if not slots:
        m = re.search(r'inherit\s+F_(ARMOR|CLOTH|HEAD_EQ|HAND_EQ|FEET_EQ|NECK_EQ|FINGER_EQ|WAIST_EQ|LEG_EQ|WRIST_EQ)', code)
        slots = [m.group(1).lower()] if m else ['armor']
    it['slot'] = slots[0] if slots[0] in SLOT else 'armor'
    it['traits'] = {SLOT.get(k, k): v for k, v in tr.items()}
    return it


def stat_effects(code):
    eff = []
    for fn, st, n in re.findall(r'(supplement_stat|consume_stat|damage_stat|heal_stat)\(\s*"(\w+)"\s*,\s*(-?\d+)', code):
        label = zh(st)
        if fn == 'supplement_stat' or fn == 'heal_stat':
            eff.append(f'{label} +{n}')
        elif fn == 'consume_stat':
            eff.append(f'{label} -{n}')
        else:
            eff.append(f'{label}實格 -{n}')
    return eff


def item_category(p, code):
    rel = str(p.relative_to(MUD))
    if 'F_FOOD' in code or set_int(code, 'food_stuff') or 'food_remaining' in code:
        return '食物'
    if 'F_LIQUID' in code or 'liquid' in code:
        return '飲品'
    if '/drug/' in rel or '/medication/' in rel or 'pill' in rel or re.search(r'"(pill|drug|herb|medicine)"', code) or '丹' in (name_ids(code)[0] or ''):
        return '丹藥'
    if 'F_STUDY' in code or '/books/' in rel:
        return '書籍'
    if '/money/' in rel or 'MONEY' in code:
        return '錢幣'
    if 'CONTAINER' in code:
        return '容器'
    return '其他'


def parse_item(p, src, code):
    it = common(p, src, code)
    it['cat'] = item_category(p, code)
    eff = stat_effects(code)
    fs = set_int(code, 'food_stuff')
    if fs:
        eff.insert(0, f'食物 +{fs}')
    if eff:
        it['effects'] = eff
    content = set_value(code, 'content')
    if content:
        it['study'] = [[zh(k), v] for k, v in mapping_pairs(content)]
    return it


def parse_npc(p, src, code):
    it = common(p, src, code)
    it['race'] = (re.search(r'set_race\(\s*"(\w+)"', code) or [None, None])[1]
    it['class'] = (re.search(r'set_class\(\s*"(\w+)"', code) or [None, None])[1]
    lv = re.search(r'set_level\(\s*(\d+)', code)
    it['level'] = int(lv.group(1)) if lv else None
    it['gender'] = set_str(code, 'gender')
    it['title'] = set_str(code, 'title')
    eq = []
    for m in re.finditer(r'carry_object\(\s*([^;]*?)\)\s*(->\s*(wield|wear)\(([^)]*)\))?\s*;', code):
        path = resolve(m.group(1), it['id'])
        if path:
            eq.append({'path': path, 'use': m.group(3) or ''})
    it['equip'] = eq
    skills = [[zh(k), int(v), k] for k, v in re.findall(r'set_skill\(\s*"([^"]+)"\s*,\s*(\d+)', code)]
    it['skills'] = skills
    it['wander'] = 'random_move' in code
    it['ghost'] = 'std_ghost' in code
    return it


def func_body(code, fn):
    m = re.search(r'\b%s\s*\([^)]*\)\s*\{' % re.escape(fn), code)
    if not m:
        return ''
    depth, i = 1, m.end()
    while i < len(code) and depth:
        depth += {'{': 1, '}': -1}.get(code[i], 0)
        i += 1
    return code[m.end():i]


def special_moves(code, rid):
    """用指令進出的路線：add_action 的處理函式裡把玩家 move 到其他房間，例如 climb 圍牆。"""
    out = []
    for m in re.finditer(r'add_action\(\s*"(\w+)"\s*,\s*(\(\{[^}]*\}\)|"[^"]*")\s*\)', code):
        body = func_body(code, m.group(1))
        verbs = re.findall(r'"([^"]+)"', m.group(2))
        args = re.findall(r'\barg\s*(?:!=|==)\s*"([^"]+)"', body)
        for t in re.finditer(r'(?:this_player\(\)|\bme|\bwho|\bob)\s*->\s*move\(\s*((?:__DIR__\s*)?"[^"]*")\s*\)', body):
            to = resolve(t.group(1), rid)
            if to:
                cmd = verbs[0] + (' ' + args[0] if args else '')
                out.append({'cmd': cmd, 'to': to})
    return out


def parse_room(p, src, code):
    rid = mud_path(p)
    ex = {}
    ev = set_value(code, 'exits') or ''
    for m in re.finditer(r'"([^"]+)"\s*:\s*((?:__DIR__\s*)?"[^"]*"|STOCK_\w+\([^)]*\))', ev):
        t = resolve(m.group(2), rid)
        if t:
            ex[m.group(1)] = t
    return {
        'id': rid,
        'file': str(p.relative_to(ROOT)).replace('\\', '/'),
        'short': re.sub(r'\x1b\[[0-9;]*m', '', set_str(code, 'short') or p.stem),
        'long': (set_str(code, 'long') or '').rstrip('\n'),
        'exits': ex,
        'objects': path_mapping(set_value(code, 'objects'), rid),
        'special': special_moves(code, rid),
        'area': set_str(code, 'map/area'),
        'layer': set_str(code, 'map/layer'),
    }


# --------------------------------------------------------------------------- 技能

# 技能「實裝」判定：技能檔以外，有 NPC、書籍、裝備、房間等遊戲內容用到這個技能代碼才算實裝。
# 排除只是登記或載入用的檔案：技能檔本身、NPC 技能預設表、升級規則、武功引擎、範例、技能目錄裡的載入入口。
GAME_REF_SKIP = ('daemon/skill/', 'daemon/misc/npc_skill.c', 'feature/char/skill.c', 'std/martial_art',
                 'obj/example/', '/skill/')


def game_refs():
    refs = {}
    for p in MUD.rglob('*.c'):
        rel = str(p.relative_to(MUD)).replace('\\', '/')
        if any(k in rel for k in GAME_REF_SKIP):
            continue
        for code in set(re.findall(r'"([a-z][a-z _\-]*)"', read(p))):
            refs.setdefault(code, []).append(rel)
    return refs


def hand_growth():
    """docs/catalog/玩家圖鑑_技能.md 手寫的精氣神與屬性成長：技能代碼 -> (精氣神, 屬性)。"""
    out = {}
    doc = ROOT / 'docs' / 'catalog' / '玩家圖鑑_技能.md'
    if not doc.exists():
        return out
    for line in read(doc).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) in (4, 5) and cols[0] and not cols[0].startswith(('技能代碼', '---')) and line.startswith('|'):
            out[cols[0]] = (cols[2], cols[3])
    return out


def attr_growth_map():
    src = read(MUD / 'feature' / 'char' / 'skill.c')
    m = re.search(r'ATTR_GROWTH\s*=\s*\(\[(.*?)\]\)', src, re.S)
    return dict(re.findall(r'"([^"]+)"\s*:\s*"(\w+)"', m.group(1))) if m else {}


SCORE_ZH = {'martial art': '武術造詣', 'martial mastery': '武學之道', 'cultivation': '修為', 'buddhology': '佛學',
            'alchemy': '丹道造詣', 'literature': '文學', 'literature mastery': '文學之道', 'combat': '戰鬥', 'magic': '法術'}
ATTR_ZH = {'str': '膂力', 'cor': '膽識', 'int': '悟性', 'spi': '靈性', 'cps': '定力', 'dex': '機敏', 'con': '根骨', 'wis': '慧根'}


def base_threshold(level):
    base = 100 if level <= 60 else 125 if level <= 90 else 150 if level <= 120 else 175 if level <= 160 else 200 if level <= 180 else 250
    return level * level * base


def skill_formula(code, src, grow_doc, attr_map, engine):
    """這項技能自己的規則：升級門檻實際數字、武功引擎參數、屬性成長、精氣神與其他成長。"""
    entry = re.search(r'query_entry_level\(\)\s*\{\s*return\s*(\d+)', src) or re.search(r'\bart_entry\s*=\s*(\d+)', src)
    ethr = re.search(r'query_entry_threshold\(\)\s*\{\s*return\s*(\d+)', src) or re.search(r'\bart_entry_threshold\s*=\s*(\d+)', src)
    pct = re.search(r'query_threshold_percent\(\)\s*\{\s*return\s*(\d+)', src)
    e = int(entry.group(1)) if entry and int(entry.group(1)) > 1 else 1
    et = int(ethr.group(1)) if ethr and int(ethr.group(1)) > 0 else 0
    pc = int(pct.group(1)) if pct and int(pct.group(1)) > 0 else 100
    levels = sorted({e, 10, 30, 50, 100, 150, 200} - set(range(1, e)))
    pts = []
    for lv in levels:
        v = et if (lv == e and et) else base_threshold(lv) * pc // 100
        pts.append(f'{lv} 級 {v:,}')
    lines = ['【升級門檻】' + ('學成等級 %d 級' % e if e > 1 else '從 1 級開始') + ('；學成需累積 %s 點' % format(et, ',') if et else '')
             + ('；門檻打 %d%%' % pc if pc != 100 else '')]
    lines.append('累積點數：' + '、'.join(pts))
    cap_m = re.search(r'query_skill_cap\([^)]*\)\s*\{[^}]*return\s*(\d+)', src)
    if cap_m:
        lines.append(f'本技能等級暫時上限 {cap_m.group(1)} 級')
    # 武功引擎參數
    if engine:
        g = lambda k: (re.search(r'\b%s\s*=\s*(-?\d+)' % k, src) or [None, None])[1]
        ma, ms, mn, mc, ab = g('ma_coef'), g('mm_start'), g('mm_minus'), g('mm_coef'), g('art_bonus')
        eng = []
        if ab is not None:
            eng.append(f'整套傷害加成 {ab}%')
        if ma and int(ma) > 0:
            eng.append(f'升到第 lv 級給武術造詣 lv × {ma}')
        if ms and mc and int(mc) > 0:
            eng.append(f'lv ≥ {ms} 時再給武學之道 (lv − {mn}) × {mc}')
        if eng:
            lines.append('【武功引擎】' + '；'.join(eng))
        mv = re.findall(r'"name":\s*"([^"]+)",\s*"lvl":\s*(\d+),\s*"attack":\s*(-?\d+),\s*"defense":\s*(-?\d+),\s*"force":\s*(-?\d+),\s*"bonus":\s*(-?\d+)', src)
        if mv:
            lines.append('【招式】名稱（需求等級；攻 / 防 / 力 / 加成）')
            for n, lv, a, d, f, b in mv:
                lines.append(f'  {n}（{lv} 級；{a} / {d} / {f} / {b}）')
    else:
        # 舊寫法：升級時直接給的造詣
        sc = []
        for name, expr in re.findall(r'gain_score\(\s*"([^"]+)"\s*,\s*([^;]*?)\)\s*;', src):
            sc.append(f'{SCORE_ZH.get(name, name)} {expr.strip()}')
        if sc:
            lines.append('【升級獎勵】' + '；'.join(sc))
    # 屬性成長（共用規則）
    if code in attr_map:
        a = ATTR_ZH.get(attr_map[code], attr_map[code])
        lines.append(f'【屬性成長】技能每到 5 的倍數擲一次，成功則{a} +1（上限 50）；機率 = 該屬性目前值的基礎機率 × (100 + 等級) ÷ 200 × 1.2。'
                     '基礎機率：屬性 1~10 100%、11~15 90%、16~20 80%、21~25 60%、26~30 40%、31~40 30%、41~45 15%、46 以上 10%。每個門檻一生只擲一次。')
    return '\n'.join(lines)


def skill_growth(code, src, grow_doc, attr_map):
    """精氣神與屬性：回傳 (精氣神說明, 屬性說明)。手寫表優先，沒有的由程式整理。"""
    gp, ga = grow_doc.get(code) or grow_doc.get(code.replace('_', ' ')) or ('', '')
    if not gp:
        pools = sorted(set(re.findall(r'advance_stat\(\s*"(\w+)"', src)))
        if pools:
            gp = '、'.join({'gin': '精', 'kee': '氣', 'sen': '神'}.get(x, x) for x in pools) + '上限會隨等級成長'
    attrs = [ATTR_ZH.get(a, a) for a in re.findall(r'set_attr\(\s*"(\w+)"', src)]
    if not ga:
        parts = []
        if code in attr_map:
            parts.append(f'每練到 5 的倍數有機會提升{ATTR_ZH.get(attr_map[code], attr_map[code])} 1 點')
        if attrs:
            parts.append('達到特定等級時：' + '、'.join(sorted(set(attrs))))
        ga = '；'.join(parts)
    return gp or '無', ga or '無'


def parse_skills(used_codes):
    # 技能升級規則（feature/char/skill.c）：點數累積在 learned，下 gain 時才判定，
    # 一次最多升一級；門檻 = 等級² × 基數，所有技能上限 200 級。
    # 技能檔可用 query_entry_level／query_entry_threshold／query_threshold_percent，
    # 或新版武功引擎的 art_entry／art_entry_threshold 指定學成等級與門檻。
    base = read(MUD / 'feature' / 'char' / 'skill.c')
    cap_m = re.search(r'int restored_skill_cap\([^)]*\)\s*\{\s*return\s*(\d+)', base)
    cap = int(cap_m.group(1)) if cap_m else 200
    formula = ('拿到的點數先累積，下 gain 時才判定升級，一次最多升一級。'
               '升到某一級所需累積點數 = 等級² × 基數；基數：1–60 級 100、61–90 級 125、91–120 級 150、'
               f'121–160 級 175、161–180 級 200、181 級以上 250。上限 {cap} 級。')
    reg = {}
    doc = ROOT / 'docs' / 'SKILL_NAMES.md'
    if doc.exists():
        for cat, name, code in re.findall(r'^\| ([^|]+) \| ([^|]+) \| `([^`]+)` \|', read(doc), re.M):
            if cat.strip() in ('分類',):
                continue
            reg[code] = (cat.strip(), name.strip())
    skills = {}
    refs = game_refs()
    grow_doc = hand_growth()
    attr_map = attr_growth_map()
    for p in sorted((MUD / 'daemon' / 'skill').glob('*.c')):
        src = read(p)
        entry_m = re.search(r'^inherit\s+"(/d/[^"]+/skill/[^"]+)"\s*;', strip_comments(src), re.M)
        if entry_m and (MUD / (entry_m.group(1).lstrip('/') + '.c')).exists():
            # 只是載入入口的技能檔：技能程式本體在 d/<區域>/skill/，代碼用空格寫法。
            p = MUD / (entry_m.group(1).lstrip('/') + '.c')
            src = read(p)
            sid = re.search(r'#define\s+SKILL_ID\s+"([^"]+)"', src)
            if sid:
                src = src.replace('register_skill_daemon(SKILL_ID)', 'register_skill_daemon("%s")' % sid.group(1))
            elif 'register_skill_daemon' not in src:
                src += '\nregister_skill_daemon("%s")' % Path(p).stem.replace('_', ' ')
        code = p.stem.replace('_', ' ') if ' ' not in p.stem and p.stem in ('secondhand_axe',) else p.stem
        m = re.search(r'register_skill_daemon\("([^"]+)"\)', src)
        if m:
            code = m.group(1)
        elif '_' in p.stem and p.stem.split('_')[0] in ('secondhand', 'twohanded'):
            code = p.stem.replace('_', ' ')
        desc = re.search(r'description:\s*(.+)', src)
        typ = re.search(r'string type\(\)\s*\{\s*return\s*"(\w+)"', src)
        notes = [c.strip() for c in re.findall(r'/\*(.*?)\*/', src, re.S)
                 if re.search(r'gin|kee|sen|STR|COR|CON|INT|growth|成長|threshold|升級', c) and 'description:' not in c]
        grow = []
        for st, n in re.findall(r'advance_stat\(\s*"(\w+)"\s*,\s*(\d+)', src):
            grow.append(f'{zh(st)} +{n}')
        form = formula
        entry = re.search(r'query_entry_level\(\)\s*\{\s*return\s*(\d+)', src) or re.search(r'\bart_entry\s*=\s*(\d+)', src)
        entry_thr = re.search(r'query_entry_threshold\(\)\s*\{\s*return\s*(\d+)', src) or re.search(r'\bart_entry_threshold\s*=\s*(\d+)', src)
        pct = re.search(r'query_threshold_percent\(\)\s*\{\s*return\s*(\d+)', src)
        extra = []
        if entry and int(entry.group(1)) > 1:
            extra.append(f'從 0 級直接學成到 {entry.group(1)} 級')
        if entry_thr and int(entry_thr.group(1)) > 0:
            extra.append(f'學成所需累積點數 {int(entry_thr.group(1)):,}')
        if pct and int(pct.group(1)) != 100:
            extra.append(f'門檻打 {pct.group(1)}%')
        if extra:
            form += '\n本技能：' + '；'.join(extra) + '。'
        code_src = strip_comments(src)
        gp, ga = skill_growth(code, code_src, grow_doc, attr_map)
        live = code in refs
        skills[code] = {
            'code': code, 'name': zh(code), 'impl': True,
            'status': 'live' if live else 'written',
            'users': sorted(refs.get(code, []))[:6],
            'cat': reg.get(code, ('', ''))[0] or ({'spell': '法術', 'martial': '武學'}.get(typ.group(1), typ.group(1)) if typ else ''),
            'desc': desc.group(1).strip() if desc else '',
            'formula': skill_formula(code, code_src, grow_doc, attr_map, 'inherit "/std/martial_art"' in src),
            'common': form, 'gp': gp, 'ga': ga,
            'growth': sorted(set(grow)), 'notes': notes,
            'file': str(p.relative_to(ROOT)).replace('\\', '/'),
        }
    # 沒有技能檔、而是用指令施展的技能（例如 berserk：cmds/std/berserk.c）。
    for p in sorted((MUD / 'cmds' / 'std').glob('*.c')):
        code = p.stem
        src = read(p)
        if code in skills or f'query_skill("{code}"' not in src:
            continue
        learn = f'query_learn("{code}"' in src
        skills[code] = {
            'code': code, 'name': reg.get(code, ('', zh(code)))[1] or zh(code), 'impl': True,
            'status': 'live' if code in refs else 'written', 'users': sorted(refs.get(code, []))[:6],
            'common': formula, 'gp': skill_growth(code, strip_comments(src), grow_doc, attr_map)[0],
            'ga': skill_growth(code, strip_comments(src), grow_doc, attr_map)[1],
            'cat': reg.get(code, ('', ''))[0] or '指令技能',
            'desc': f'用指令 {code} 施展。' + ('需要有這項技能的學習點數才算學會（只用巫師指令設定等級、沒有學習點數，會顯示「你不會這項技能」）。' if learn else ''),
            'formula': skill_formula(code, strip_comments(src), grow_doc, attr_map, False), 'growth': [], 'notes': [],
            'file': str(p.relative_to(ROOT)).replace('\\', '/'),
        }
    for code in sorted(set(reg) | used_codes):
        if code in skills:
            if code in reg and not skills[code]['cat']:
                skills[code]['cat'] = reg[code][0]
            continue
        skills[code] = {'code': code, 'name': reg.get(code, ('', zh(code)))[1] or zh(code), 'impl': False,
                        'status': 'name', 'users': [], 'common': '', 'gp': '', 'ga': '',
                        'cat': reg.get(code, ('', ''))[0], 'desc': '', 'formula': '尚未實作（目前只有代碼與名稱）。',
                        'growth': [], 'notes': [], 'file': ''}
    return sorted(skills.values(), key=lambda s: (['live', 'written', 'name'].index(s['status']), s['cat'] or '~', s['code']))


# --------------------------------------------------------------------------- 地圖座標

def layout(rooms, ids):
    """依出口方向替區域內的房間排座標；衝突時往旁邊找空位。"""
    pos = {}
    used = set()
    comp_x = 0
    ids = sorted(ids)
    order = sorted(ids, key=lambda r: (-len(rooms[r]['exits']), r))
    for start in order:
        if start in pos:
            continue
        q = deque([start])
        cx = comp_x
        pos[start] = (cx, 0, 0)
        used.add(pos[start])
        comp = [start]
        while q:
            cur = q.popleft()
            x, y, z = pos[cur]
            for d, t in sorted(rooms[cur]['exits'].items()):
                if t not in ids or t in pos:
                    continue
                dx, dy, dz = DIRS.get(d, (0, 0, 0))
                if (dx, dy, dz) == (0, 0, 0):
                    dx = 1
                cand = (x + dx, y + dy, z + dz)
                if cand in used:
                    for r in range(1, 6):
                        found = None
                        for ox in range(-r, r + 1):
                            for oy in range(-r, r + 1):
                                c = (cand[0] + ox, cand[1] + oy, cand[2])
                                if c not in used:
                                    found = c
                                    break
                            if found:
                                break
                        if found:
                            cand = found
                            break
                pos[t] = cand
                used.add(cand)
                comp.append(t)
                q.append(t)
        xs = [pos[r][0] for r in comp]
        comp_x = max(xs) + 2
    return pos


# --------------------------------------------------------------------------- 主程式

def collect():
    """掃描 mudlib，傳回圖鑑資料（巫師版與玩家版共用）。"""
    weapons, armors, items, npcs, rooms = [], [], [], {}, {}
    for d in SCAN_DIRS:
        for p in sorted((MUD / d).rglob('*.c')):
            src = read(p)
            code = strip_comments(src)
            kind = classify(code)
            try:
                if kind == 'room':
                    r = parse_room(p, src, code)
                    rooms[r['id']] = r
                elif kind == 'npc':
                    n = parse_npc(p, src, code)
                    npcs[n['id']] = n
                elif kind == 'weapon':
                    weapons.append(parse_weapon(p, src, code))
                elif kind == 'armor':
                    armors.append(parse_armor(p, src, code))
                elif kind == 'item':
                    items.append(parse_item(p, src, code))
            except Exception as e:  # 單一檔案解析失敗不影響整體
                print('skip', p, e, file=sys.stderr)

    # 區域：有 map/area 的依名稱分組，沒有的依所在目錄
    areas = defaultdict(list)
    for rid in [r for r in rooms if r.startswith('/obj/') or r.startswith(tuple(HIDDEN_DIRS))]:
        del rooms[rid]  # 系統用的空房間與不公開區域
    for r in rooms.values():
        r['special'] = [m for m in r['special'] if m['to'] in rooms]
    hidden = lambda i: i.startswith(tuple(HIDDEN_DIRS))
    placed = {p for r in rooms.values() for p in r['objects']}
    for nid in [n for n in npcs if hidden(n) and n not in placed]:
        del npcs[nid]
    carried = placed | {e['path'] for n in npcs.values() for e in n['equip']}
    weapons[:] = [w for w in weapons if not hidden(w['id']) or w['id'] in carried]
    armors[:] = [a for a in armors if not hidden(a['id']) or a['id'] in carried]
    items[:] = [i for i in items if not hidden(i['id']) or i['id'] in carried]
    for r in rooms.values():
        key = r['area'] or DIR_AREA.get(r['id'].rsplit('/', 2)[0].lstrip('/'), r['id'].rsplit('/', 2)[0].lstrip('/'))
        r['areaKey'] = key
        areas[key].append(r['id'])
    npc_rooms = defaultdict(list)
    item_rooms = defaultdict(list)
    for r in rooms.values():
        for path, n in r['objects'].items():
            if path in npcs:
                npc_rooms[path].append([r['id'], n])
            else:
                item_rooms[path].append([r['id'], n])
    for n in npcs.values():
        n['rooms'] = npc_rooms.get(n['id'], [])
    area_list = []
    for key in list(areas):
        areas[key] = [r for r in areas[key] if r not in MAP_HIDE]
    for key, ids in areas.items():
        manual = {r for r in ids if r in MAP_POS}
        pos = layout(rooms, set(ids) - manual)
        pending = sorted(manual)
        while pending:  # 參考房間也可能是手動指定的，依序解開
            left = []
            for rid in pending:
                ref, dx, dy, *dz = MAP_POS[rid]
                if ref in pos:
                    x, y, z = pos[ref]
                    pos[rid] = (x + dx, y + dy, z + (dz[0] if dz else 0))
                else:
                    left.append(rid)
            if len(left) == len(pending):
                for rid in left:
                    pos[rid] = (0, 0, 0)
                break
            pending = left
        for rid in ids:
            rooms[rid]['pos'] = pos[rid]
        npc_ids = sorted({p for rid in ids for p in rooms[rid]['objects'] if p in npcs})
        area_list.append({'key': key, 'name': key, 'rooms': sorted(ids), 'npcs': npc_ids})
    area_list.sort(key=lambda a: -len(a['rooms']))

    # 技能清單：技能程式、docs/SKILL_NAMES.md，加上武器用法、裝備特性與 NPC 用到的技能代碼
    not_skill = set(NAMED) | {'intimidate', 'wittiness', 'awarness', 'move', 'vision_of_ghost', 'resist_poison'}
    used_codes = set()
    for w in weapons:
        used_codes.update(u['code'] for u in w['usages'])
    for coll in (weapons, armors):
        for it in coll:
            for lst in it['traits'].values():
                used_codes.update(k for _, _, k in lst)
    for n in npcs.values():
        used_codes.update(k for _, _, k in n['skills'])
    skills = parse_skills({c for c in used_codes if c and c not in not_skill and not c.startswith(('armor_vs', 'damage_vs'))})
    data = {
        'skills': skills,
        'weapons': sorted(weapons, key=lambda w: (KIND_ORDER.index(w['kind']) if w['kind'] in KIND_ORDER else 99, w['id'])),
        'armors': sorted(armors, key=lambda a: (SLOT_ORDER.index(a['slot']) if a['slot'] in SLOT_ORDER else 99, a['id'])),
        'items': sorted(items, key=lambda i: (i['cat'], i['id'])),
        'npcs': npcs,
        'rooms': rooms,
        'areas': area_list,
        'itemRooms': item_rooms,
        'weaponKinds': WEAPON_KIND,
        'slots': SLOT,
        'counts': {'skills': len(skills), 'weapons': len(weapons), 'armors': len(armors), 'items': len(items),
                   'npcs': len(npcs), 'rooms': len(rooms), 'areas': len(area_list)},
    }
    return data


def main(out):
    data = collect()
    blob = json.dumps(data, ensure_ascii=False, separators=(',', ':')).replace('</', '<\\/')
    html = read(TEMPLATE).replace('/*__CATALOG_DATA__*/null', blob, 1)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(html, encoding='utf-8')
    print(json.dumps(data['counts'], ensure_ascii=False), '->', out)


if __name__ == '__main__':
    main(Path(sys.argv[1]) if len(sys.argv) > 1 else OUT)
