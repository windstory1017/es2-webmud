#!/usr/bin/env python3
"""檢查遊戲文字有沒有 Big5（CP950）沒有的字。設計文件：docs/ZMUD_TELNET_PLAN.md

zMUD 4.62 只看得懂 Big5；遇到沒有的字會顯示成「?」。網頁玩家看不出差別，所以要用這個工具檢查。

用法：
  python3 tools/check_big5_text.py            # 掃描並列出（有問題時結束碼 1）
  python3 tools/check_big5_text.py --fix      # 把有對照表的字自動換成兩邊都認得的字
  python3 tools/check_big5_text.py 路徑...    # 只掃指定的檔案或資料夾
"""
import os, sys, argparse

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MUDLIB = os.path.join(ROOT, 'source', 'upstream', 'mudlib')
EXTS = ('.c', '.h', '.txt')
SKIP_DIRS = ('data', 'log', 'bin')          # 玩家存檔、紀錄、編譯快取
# 程式本身要處理 Unicode 範圍的檔案，不是給玩家看的文字
SKIP_FILES = ('adm/simul_efun/utf8.c', 'feature/npc/vendor.c')   # 程式用 Unicode 範圍算字寬

# 自動替換對照表：只放意思明確的字。不在表內的字只列出，由人決定。
REPLACE = {
    '−': '-',    # − 減號
    '²': '^2',   # ² 平方（算式註解用；玩家看得到的文字請手動改寫）
    '↔': '－',   # ↔ 兩端之間的連線（如渡船航線）
    '⋯': '…',   # ⋯ 刪節號
    '噼': '劈',   # 劈哩啪啦
    '换': '換',   # 簡體
    '癯': '瘦',   # 清瘦
}

def encodable(ch):
    try:
        ch.encode('cp950'); return True
    except UnicodeEncodeError:
        return False

def iter_files(paths):
    for base in paths:
        if os.path.isfile(base):
            yield base; continue
        for r, dirs, fs in os.walk(base):
            rel = os.path.relpath(r, MUDLIB)
            if rel.split(os.sep)[0] in SKIP_DIRS and r.startswith(MUDLIB):
                dirs[:] = []; continue
            for f in fs:
                if f.endswith(EXTS):
                    yield os.path.join(r, f)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('paths', nargs='*')
    ap.add_argument('--fix', action='store_true')
    a = ap.parse_args()
    bad = 0; fixed = 0
    for p in sorted(iter_files(a.paths or [MUDLIB])):
        if os.path.relpath(p, MUDLIB).replace(os.sep, '/') in SKIP_FILES:
            continue
        try:
            text = open(p, encoding='utf-8', newline='').read()
        except (UnicodeDecodeError, OSError):
            continue
        out = []; changed = False
        for n, line in enumerate(text.split('\n'), 1):
            new = line
            for ch in sorted({c for c in line if ord(c) > 127 and not encodable(c)}):
                if a.fix and ch in REPLACE:
                    new = new.replace(ch, REPLACE[ch]); changed = True; fixed += 1
                else:
                    bad += 1
                    print(f'{os.path.relpath(p, ROOT)}:{n}: U+{ord(ch):04X} {ch} 　{line.strip()[:60]}')
            out.append(new)
        if changed:
            open(p, 'w', encoding='utf-8', newline='').write('\n'.join(out))
    if a.fix:
        print(f'已替換 {fixed} 處。')
    print(f'Big5 沒有的字：{bad} 處' + ('' if bad else '（全部通過）'))
    return 1 if bad else 0

if __name__ == '__main__':
    sys.exit(main())
