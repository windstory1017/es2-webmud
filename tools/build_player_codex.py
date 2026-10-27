#!/usr/bin/env python3
"""產生給玩家看的天朝圖鑑：web/codex.html（規劃見 docs/PLAYER_CODEX_PLAN.md）。

和巫師版圖鑑（tools/build_codex.py）共用同一套 mudlib 分析，差別在：
- 只收從出生點（雪亭鎮客棧大堂）沿出口走得到的房間，以及這些房間裡的 NPC、
  NPC 身上與商店裡的東西、這些 NPC 傳授的技能。
- 不寫公式、檔案路徑與英文代碼；NPC 不顯示等級與戰力；技能只寫會增長哪些屬性。
- 要特殊動作才進得去的房間照常顯示，但不說明怎麼進去。
- 解謎才學得到的武功（docs/martial_arts/門派武功對照表.md 學習條件為「解謎」）不列。
- 門派文字讀 docs/catalog/玩家圖鑑_門派.md。

用法：
    python3 tools/build_player_codex.py                 # 輸出 web/codex.html
    python3 tools/build_player_codex.py 其他輸出路徑.html
"""
import json
import re
import sys
from collections import deque
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_codex as bc  # noqa: E402

ROOT = bc.ROOT
MUD = bc.MUD
TEMPLATE = ROOT / 'tools' / 'codex_player_template.html'
OUT = ROOT / 'web' / 'codex.html'
FACTION_DOC = ROOT / 'docs' / 'catalog' / '玩家圖鑑_門派.md'
SKILL_DOC = ROOT / 'docs' / 'catalog' / '玩家圖鑑_技能.md'
SPELL_DOC = ROOT / 'docs' / 'catalog' / '玩家圖鑑_法術.md'
HELP_DIR = MUD / 'docs' / 'help'
SKILL_TABLE = ROOT / 'docs' / 'martial_arts' / '門派武功對照表.md'
START = '/d/snow/inn_hall'

DIR_ZH = {'north': '北', 'south': '南', 'east': '東', 'west': '西', 'northeast': '東北', 'northwest': '西北',
          'southeast': '東南', 'southwest': '西南', 'up': '上', 'down': '下', 'enter': '進入', 'out': '出去',
          'northup': '北上', 'southup': '南上', 'eastup': '東上', 'westup': '西上',
          'northdown': '北下', 'southdown': '南下', 'eastdown': '東下', 'westdown': '西下'}
ATTR_ZH = {'str': '膂力', 'cor': '膽識', 'cps': '定力', 'int': '悟性', 'wis': '慧根', 'spi': '靈性',
           'con': '根骨', 'dex': '機敏', 'kar': '福緣'}
POOL_ZH = {'gin': '精', 'kee': '氣', 'sen': '神'}
# 基本技能（使用者指示，2026-10-24：左手、雙手兵刃與讀書識字、經脈穴道之學、偷襲暗算技倆、
# 古文造詣、五行陰陽之學也算基本技能）
BASIC_SKILLS = {'force', 'dodge', 'parry', 'unarmed', 'sword', 'blade', 'staff', 'axe', 'dagger', 'pike', 'whip',
                'blunt', 'needle', 'throwing', 'spells', 'magic', 'literate', 'kee anatomy', 'backstab',
                'archaic attainment', 'mysticism'}
USAGE_ZH = {'force': '內功', 'dodge': '閃躲', 'parry': '招架', 'unarmed': '徒手', 'sword': '劍術', 'blade': '刀法',
            'staff': '杖法', 'axe': '斧術', 'dagger': '錐法', 'pike': '槍法', 'whip': '鞭法', 'blunt': '鎚法',
            'needle': '針術', 'spells': '咒術', 'magic': '法術'}
TYPE_ZH = {'martial': '武學', 'magic': '法術', 'spell': '咒術', 'knowledge': '知識', 'skill': '技藝'}


def reachable(rooms):
    seen, q = {START}, deque([START])
    while q:
        r = q.popleft()
        targets = list(rooms[r]['exits'].values()) + [m['to'] for m in rooms[r]['special']]
        for t in targets:
            if t in rooms and t not in seen:
                seen.add(t)
                q.append(t)
    return seen


def faction_of(code):
    m = re.search(r'#define\s+FACTION\s+"([^"]+)"', code)
    define = m.group(1) if m else None
    m = re.search(r'set\(\s*"custom_faction"\s*,\s*(?:"([^"]+)"|FACTION)', code)
    return (m.group(1) or define) if m else None


def taught_skills(code):
    """NPC 的 acquire_skill() 裡肯傳授的技能代碼。"""
    body = bc.func_body(code, 'acquire_skill')
    if not body:
        return []
    out = re.findall(r'case\s+"([^"]+)"', body)
    out += re.findall(r'\bskill\s*==\s*"([^"]+)"', body)
    for lst in re.findall(r'member_array\(\s*skill\s*,\s*\(\{(.*?)\}\)', body, re.S):
        out += re.findall(r'"([^"]+)"', lst)
    for var in re.findall(r'member_array\(\s*skill\s*,\s*(\w+)', body):
        m = re.search(r'\b%s\s*=\s*\(\{(.*?)\}\)' % re.escape(var), code, re.S)
        if m:
            out += re.findall(r'"([^"]+)"', m.group(1))
    return list(dict.fromkeys(out))


def puzzle_skills():
    """門派武功對照表裡學習條件為「解謎」的武功，不公開。"""
    if not SKILL_TABLE.exists():
        return set()
    out = set()
    for line in bc.read(SKILL_TABLE).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) >= 5:
            code = re.search(r'`([^`]+)`', cols[2])
            if code and '解謎' in cols[4]:
                out.add(code.group(1))
    return out


def skill_lineage():
    """門派武功對照表：技能代碼 -> (師承, 類型)。"""
    out = {}
    if not SKILL_TABLE.exists():
        return out
    for line in bc.read(SKILL_TABLE).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) >= 4:
            code = re.search(r'`([^`]+)`', cols[2])
            if code:
                out.setdefault(code.group(1), (cols[0], cols[3]))
    return out


def load_factions():
    rows = []
    if not FACTION_DOC.exists():
        return rows
    for line in bc.read(FACTION_DOC).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) == 5 and cols[2].startswith('/'):
            rows.append({'name': cols[0], 'cls': cols[1], 'master': cols[2], 'entry': cols[3], 'intro': cols[4]})
    return rows


def load_skill_growth():
    """玩家圖鑑_技能.md：技能代碼 -> {entry, pools, attrs}（成長文字，不含經驗值公式）。"""
    out = {}
    if not SKILL_DOC.exists():
        return out
    for line in bc.read(SKILL_DOC).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) in (4, 5) and cols[0] and not cols[0].startswith(('技能代碼', '---')):
            out[cols[0]] = {'entry': cols[1], 'pools': cols[2], 'attrs': cols[3],
                            'owner': cols[4] if len(cols) == 5 else ''}
    return out


def load_spells():
    """玩家圖鑑_法術.md：技能代碼 -> {'note': 共通特性, 'list': [{name, lv, cost, eff, dmg}]}。"""
    out = {}
    if not SPELL_DOC.exists():
        return out
    for line in bc.read(SPELL_DOC).splitlines():
        cols = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cols) != 7 or not cols[0] or cols[0].startswith(('技能代碼', '---')):
            continue
        sk = out.setdefault(cols[0], {'note': '', 'list': []})
        if cols[1] == '（共通）':
            sk['note'] = cols[5]
        else:
            sk['list'].append({'name': cols[1], 'cmd': cols[2], 'lv': cols[3], 'cost': cols[4], 'eff': cols[5],
                               'dmg': cols[6]})
    return out


def load_help():
    """遊戲說明檔（mudlib/docs/help/*.txt）裡的技能段落：技能代碼 -> {kind, cond, start, abilities, gain}。

    格式見 CLAUDE.md「說明檔格式」：每項技能一段「◎ 名稱（id）」，依序寫技能種類、學習條件、
    任務起始、特殊能力（每項「‧」開頭）、能力提昇。門派說明檔（職業.門派.txt）是精簡版，
    優先採用；只有技能自己的說明檔（詳細版）時才用它。圖鑑照說明檔的寫法，不另外寫公式。"""
    out = {}
    if not HELP_DIR.exists():
        return out
    files = sorted(HELP_DIR.glob('*.txt'), key=lambda p: '.' not in p.stem)  # 門派說明檔排前面
    for f in files:
        cur = None
        last = None
        for line in bc.read(f).splitlines():
            m = re.match(r'^◎\s*(.+?)（([^）]+)）\s*$', line)
            if m:
                code = m.group(2).strip()
                cur = None if code in out else {'kind': '', 'cond': '', 'start': '', 'abilities': [], 'level': '', 'gain': ''}
                if cur is not None:
                    out[code] = cur
                last = None
                continue
            if cur is None:
                continue
            if not line.strip() or line.startswith(('○', '◎')):
                if not line.strip():
                    continue
                cur = None
                continue
            body = line.lstrip('　 ')
            fm = re.match(r'^(技能種類|學習條件|任務起始|特殊能力|練成等級|能力提升|能力提昇)：(.*)$', body)
            if fm and line.startswith('　') and not line.startswith('　　'):
                key = {'技能種類': 'kind', '學習條件': 'cond', '任務起始': 'start', '特殊能力': None, '練成等級': 'level', '能力提升': 'gain', '能力提昇': 'gain'}[fm.group(1)]
                if key:
                    cur[key] = fm.group(2).strip()
                last = key
            elif body.startswith('‧'):
                item = body[1:].strip()
                if item != '無':
                    cur['abilities'].append(item)
                last = 'abilities'
            elif last == 'abilities' and cur['abilities']:
                cur['abilities'][-1] += body.strip()
            elif last in ('kind', 'cond', 'start', 'level', 'gain'):
                cur[last] += body.strip()
    return out


def art_info(path):
    """新版武功：招式（名稱、幾級起可用）與給玩家看的簡介 art_desc。"""
    if not path:
        return [], ''
    code = bc.strip_comments(bc.read(ROOT / path))
    moves = []
    m = re.search(r'\bmoves\s*=\s*\(\{(.*?)\}\)\s*;', code, re.S)
    if m:
        for blk in re.findall(r'\(\[(.*?)\]\)', m.group(1), re.S):
            name = re.search(r'"name"\s*:\s*"([^"]+)"', blk)
            lvl = re.search(r'"lvl"\s*:\s*(\d+)', blk)
            if name:
                moves.append([name.group(1), int(lvl.group(1)) if lvl else 0])
    d = re.search(r'\bart_desc\s*=\s*((?:"(?:[^"\\]|\\.)*"\s*\+?\s*)+);', code)
    desc = ''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', d.group(1))) if d else ''
    return moves, desc


def entry_level(path):
    """技能檔指定的學成等級；沒有指定的是 1。"""
    if not path:
        return 1
    code = bc.strip_comments(bc.read(ROOT / path))
    m = re.search(r'query_entry_level\(\)\s*\{\s*return\s*(\w+)', code) or re.search(r'\bart_entry\s*=\s*(\w+)', code)
    if m:
        v = m.group(1)
        if not v.isdigit():  # #define ENTRY_LEVEL 70
            d = re.search(r'#define\s+%s\s+(\d+)' % re.escape(v), code)
            v = d.group(1) if d else '1'
        return max(1, int(v))
    # 只是載入入口的技能檔（inherit 本體）：讀本體
    inh = re.search(r'inherit\s+"(/[^"]+)"', code)
    if inh:
        body = MUD / (inh.group(1).lstrip('/') + ('' if inh.group(1).endswith('.c') else '.c'))
        if body.exists():
            return entry_level(str(body.relative_to(ROOT)))
    return 1


def skill_usage(path):
    """技能檔 valid_enable() 允許的第一個基本技能；載入入口檔讀它繼承的本體。"""
    if not path:
        return None
    code = bc.strip_comments(bc.read(ROOT / path))
    m = re.search(r'usage\s*==\s*"([^"]+)"', bc.func_body(code, 'valid_enable'))
    if m:
        return m.group(1)
    inh = re.search(r'inherit\s+"(/[^"]+)"', code)
    if inh:
        body = MUD / (inh.group(1).lstrip('/') + '.c')
        if body.exists():
            return skill_usage(str(body.relative_to(ROOT)))
    return None


def attr_growth_table():
    src = bc.read(MUD / 'feature' / 'char' / 'skill.c')
    m = re.search(r'ATTR_GROWTH\s*=\s*\(\[(.*?)\]\)', src, re.S)
    return dict(re.findall(r'"([^"]+)"\s*:\s*"(\w+)"', m.group(1))) if m else {}


def skill_effects(path):
    """技能檔：練成或升級時增長的屬性、精氣神上限（只取名稱，不取數字）。"""
    if not path:
        return [], []
    code = bc.strip_comments(bc.read(ROOT / path))
    attrs = re.findall(r'set_attr\(\s*"(\w+)"\s*,\s*\w+->query_attr\(\s*"\1"\s*,\s*1\s*\)\s*\+', code)
    pools = re.findall(r'advance_stat\(\s*"(\w+)"', code)
    return attrs, pools


def clean_desc(desc, name):
    """技能檔開頭的說明是寫給開發者看的；去掉括號註記、英文與程式用語，只留中文敘述。"""
    s = re.sub(r'[（(][^）)]*[）)]', '', desc or '')
    out = []
    for sent in re.split(r'[。；]', s):
        sent = sent.strip()
        if not sent or sent == name:
            continue
        parts = [p for p in re.split(r'[，、]', sent) if p.strip() and not re.search(r'[A-Za-z_/]', p)
                 and not re.search(r'設計見|公式|程式|載入|定義|取自|特技|訊息|機率', p)]
        if parts:
            out.append('，'.join(p.strip() for p in parts))
    return ('。'.join(out) + '。') if out else ''


def main(out):
    data = bc.collect()
    rooms, npcs = data['rooms'], data['npcs']
    reach = reachable(rooms) - bc.MAP_HIDE
    items_by_id = {}
    for kind in ('weapons', 'armors', 'items'):
        for it in data[kind]:
            it['kind_tab'] = kind
            items_by_id[it['id']] = it

    # ---- NPC：放在走得到的房間裡
    npc_rooms = {}
    for rid in sorted(reach):
        for p, n in rooms[rid]['objects'].items():
            if p in npcs:
                npc_rooms.setdefault(p, []).append([rid, n if isinstance(n, int) else 1])
    hidden_skills = puzzle_skills()
    extra = {}
    for nid in npc_rooms:
        code = bc.strip_comments(bc.read(ROOT / npcs[nid]['file']))
        goods = bc.path_mapping(bc.set_value(code, 'merchandise'), nid)
        teaches = taught_skills(code)
        # 會傳授解謎武功的 NPC，拜師本身就是謎題的一部分：整份傳授清單都不公開。
        if any(s in hidden_skills for s in teaches):
            teaches = []
        extra[nid] = {
            'faction': faction_of(code),
            'teaches': teaches,
            'sells': [p for p in goods if p in items_by_id],
        }

    # ---- 物品：房間地上、NPC 身上、商店
    item_src = {}
    def add_src(iid, kind, ref):
        if iid in items_by_id:
            item_src.setdefault(iid, []).append([kind, ref])
    for rid in sorted(reach):
        for p, n in rooms[rid]['objects'].items():
            if p not in npcs:
                add_src(p, 'room', rid)
    for nid in npc_rooms:
        for e in npcs[nid]['equip']:
            add_src(e['path'], 'npc', nid)
        for p in extra[nid]['sells']:
            add_src(p, 'shop', nid)

    # ---- 技能：NPC 傳授、書籍研讀
    skill_by_code = {s['code']: s for s in data['skills']}
    lineage = skill_lineage()
    growth = attr_growth_table()
    growth_doc = load_skill_growth()
    spell_doc = load_spells()
    help_doc = load_help()
    teach_of, book_of = {}, {}
    for nid in npc_rooms:
        for s in extra[nid]['teaches']:
            teach_of.setdefault(s, []).append(nid)
    # 書籍的 study 欄位是 [中文名稱, 上限]；英文代碼要從原始碼再取一次
    for iid in item_src:
        it = items_by_id[iid]
        if it.get('study'):
            code = bc.strip_comments(bc.read(ROOT / it['file']))
            for k, _ in bc.mapping_pairs(bc.set_value(code, 'content')):
                if k not in hidden_skills:
                    book_of.setdefault(k, []).append(iid)

    # ---- 門派
    factions = []
    for f in load_factions():
        if f['master'] not in npc_rooms:
            continue
        code = extra[f['master']]['faction']
        factions.append({**f, 'code': code})

    # ---- 編號：不把檔案路徑放進網頁
    ids = {}
    def nid_of(prefix, key):
        if key not in ids:
            ids[key] = f'{prefix}{len([k for k in ids if ids[k].startswith(prefix)]) + 1}'
        return ids[key]
    for rid in sorted(reach):
        nid_of('r', rid)
    for n in sorted(npc_rooms):
        nid_of('n', n)
    for i in sorted(item_src):
        nid_of('i', i)
    skill_codes = sorted(set(teach_of) | set(book_of))
    skill_codes = [c for c in skill_codes if c in skill_by_code or bc.zh(c) != c]
    for c in skill_codes:
        nid_of('s', c)
    for i, f in enumerate(factions):
        f['id'] = f'f{i + 1}'

    def text(s):
        return (s or '').rstrip('\n')

    # 地圖
    areas = []
    start_area = rooms[START]['areaKey']
    for a in sorted(data['areas'], key=lambda a: a['key'] != start_area):  # 出生地所在區域排第一
        rs = [r for r in a['rooms'] if r in reach]
        if rs:
            areas.append({'key': a['key'], 'name': a['name'], 'rooms': [ids[r] for r in rs]})
    out_rooms = {}
    for rid in reach:
        r = rooms[rid]
        objs = r['objects']
        out_rooms[ids[rid]] = {
            'short': r['short'], 'long': text(r['long']), 'pos': r.get('pos', (0, 0, 0)), 'layer': r['layer'],
            'exits': {DIR_ZH.get(d, d): ids[t] for d, t in r['exits'].items() if t in reach},
            'ud': [d for d in ('up', 'down') if d in r['exits']],
            'npcs': [[ids[p], n if isinstance(n, int) else 1] for p, n in objs.items() if p in npc_rooms],
            'items': [[ids[p], n if isinstance(n, int) else 1] for p, n in objs.items() if p in item_src],
        }

    # 門派代碼 -> 門派編號（NPC 所屬門派）
    fac_by_code = {f['code']: f['id'] for f in factions if f['code']}
    fac_by_master = {f['master']: f['id'] for f in factions}

    out_npcs = {}
    for n, rlist in npc_rooms.items():
        src = npcs[n]
        ex = extra[n]
        out_npcs[ids[n]] = {
            'name': src['name'], 'title': src.get('title'), 'ids': src.get('ids', [])[:1], 'long': text(src['long']),
            'rooms': [[ids[r], c] for r, c in rlist], 'wander': src.get('wander', False),
            'equip': [[ids[e['path']], e['use']] for e in src['equip'] if e['path'] in item_src],
            'sells': [ids[p] for p in ex['sells']],
            'teaches': [ids[s] for s in ex['teaches'] if s in ids],
            'faction': fac_by_master.get(n) or fac_by_code.get(ex['faction']),
        }

    def item_out(it):
        o = {'id': ids[it['id']], 'name': it['name'], 'ids': it.get('ids', [])[:1], 'long': text(it['long']),
             'value': it.get('value'), 'weight': it.get('weight'),
             'flags': [f for f in it.get('flags', []) if f != '可研讀'],
             'src': [[k, ids[ref]] for k, ref in item_src[it['id']]]}
        if it['kind_tab'] == 'weapons':
            o['kind'] = it['kind']
            o['usages'] = [{'skill': u['skill'], 'dmg': u.get('dmg')} for u in it['usages']]
            o['traits'] = {k: [[a, v] for a, v, _ in l] for k, l in it['traits'].items()}
        elif it['kind_tab'] == 'armors':
            o['slot'] = it['slot']
            o['traits'] = {k: [[a, v] for a, v, _ in l] for k, l in it['traits'].items()}
        else:
            o['cat'] = it['cat']
            if it.get('effects'):
                o['effects'] = it['effects']
            if it.get('study'):
                o['study'] = [ids[k] for k in book_of if it['id'] in book_of[k] and k in ids]
        return o

    out_items = {k: [item_out(it) for it in data[k] if it['id'] in item_src] for k in ('weapons', 'armors', 'items')}

    out_skills = []
    for c in skill_codes:
        s = skill_by_code.get(c) or {}
        if not s.get('file') and skill_by_code.get(c.replace(' ', '_'), {}).get('file'):
            s = dict(skill_by_code[c.replace(' ', '_')], name=s.get('name') or bc.zh(c))  # 只有載入入口檔的技能
        attrs, pools = skill_effects(s.get('file'))
        if c in growth:
            attrs = [growth[c]] + attrs
        lin = lineage.get(c)
        teachers = teach_of.get(c, [])
        facs = sorted({out_npcs[ids[t]]['faction'] for t in teachers if out_npcs[ids[t]]['faction']})
        cat = (lin[1] if lin else '') or s.get('cat') or ''
        cat = TYPE_ZH.get(cat, cat)
        if cat == '指令技能':
            cat = '技藝'
        moves, art_desc = art_info(s.get('file'))
        desc = art_desc or clean_desc(s.get('desc'), s.get('name') or bc.zh(c))
        basic = c in BASIC_SKILLS or c.startswith(('secondhand ', 'twohanded '))
        if basic:
            cat = '基本技能'
        elif not cat:
            # 沒有登記分類的武功：看它能裝在哪一項基本技能上（例如內功）
            usage = skill_usage(s.get('file'))
            cat = USAGE_ZH.get(usage, '') if usage else ''
        gd = growth_doc.get(c, {})
        entry = gd.get('entry') or f'{entry_level(s.get("file"))} 級'
        if re.fullmatch(r'\d+', entry):
            entry += ' 級'
        # 歸屬：寫門派名稱就歸到那個門派；寫職業名稱則顯示「某某專屬」
        owner, excl = gd.get('owner', ''), ''
        if owner:
            fid = next((f['id'] for f in factions if f['name'] == owner), None)
            if fid:
                facs = [fid]
            else:
                facs, excl = [], owner + '專屬'
        attr_text = gd.get('attrs') or (f'每練到 5 的倍數有機會提升{ATTR_ZH[growth[c]]} 1 點（屬性最高 50）。'
                                        if growth.get(c) in ATTR_ZH else '')
        out_skills.append({
            'entry': entry, 'poolText': gd.get('pools', ''), 'attrText': attr_text,
            'spellNote': spell_doc.get(c, {}).get('note', ''), 'spells': spell_doc.get(c, {}).get('list', []),
            'id': ids[c], 'name': s.get('name') or bc.zh(c), 'cat': cat, 'basic': basic,
            'desc': desc,
            'attrs': list(dict.fromkeys(ATTR_ZH.get(a, a) for a in attrs if a in ATTR_ZH)),
            'pools': list(dict.fromkeys(POOL_ZH[p] for p in pools if p in POOL_ZH)),
            'teachers': [ids[t] for t in teachers], 'books': [ids[b] for b in book_of.get(c, []) if b in ids],
            'factions': facs, 'excl': excl, 'moves': moves, 'help': help_doc.get(c),
        })
    # 單一門派的武功依門派排在前面，各派共通的基本功夫與書上學得到的放後面。
    fac_order = {f['id']: i for i, f in enumerate(factions)}
    out_skills.sort(key=lambda s: (s['basic'] or len(s['factions']) != 1, fac_order.get((s['factions'] or [''])[0], 99),
                                   s['basic'], not s['excl'], s['cat'], s['name']))

    out_factions = []
    for f in factions:
        masters = [ids[f['master']]]
        skills = [ids[s] for s in extra[f['master']]['teaches'] if s in ids]
        # 歸屬於本門、但不是掌門親傳的武功（例如竹上經心法由大弟子傳授）
        skills += [k['id'] for k in out_skills if k['factions'] == [f['id']] and k['id'] not in skills]
        out_factions.append({'id': f['id'], 'name': f['name'], 'cls': f['cls'], 'entry': f['entry'], 'intro': f['intro'],
                             'masters': masters, 'members': [k for k, v in out_npcs.items() if v['faction'] == f['id']],
                             'skills': skills})

    pdata = {
        'areas': areas, 'rooms': out_rooms, 'npcs': out_npcs, 'factions': out_factions, 'skills': out_skills,
        'weapons': out_items['weapons'], 'armors': out_items['armors'], 'items': out_items['items'],
        'weaponKinds': bc.WEAPON_KIND, 'slots': bc.SLOT,
        'counts': {'rooms': len(out_rooms), 'areas': len(areas), 'npcs': len(out_npcs), 'factions': len(out_factions),
                   'skills': len(out_skills), 'weapons': len(out_items['weapons']), 'armors': len(out_items['armors']),
                   'items': len(out_items['items'])},
    }
    blob = json.dumps(pdata, ensure_ascii=False, separators=(',', ':')).replace('</', '<\\/')
    html = bc.read(TEMPLATE).replace('/*__CODEX_DATA__*/null', blob, 1)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(html, encoding='utf-8')
    print(json.dumps(pdata['counts'], ensure_ascii=False), '->', out)


if __name__ == '__main__':
    main(Path(sys.argv[1]) if len(sys.argv) > 1 else OUT)
