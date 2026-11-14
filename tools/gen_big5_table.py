#!/usr/bin/env python3
"""產生 daemon/misc/big5_d.c：Big5（CP950）有的字的字表，給 name 指令檢查玩家取的名稱用。
設計文件：docs/EMBROIDERY.md。標準與 tools/check_big5_text.py 相同（Python 的 cp950）。
用法：python3 tools/gen_big5_table.py
"""
import os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'source', 'upstream', 'mudlib', 'daemon', 'misc', 'big5_d.c')

chars = []
for cp in range(0x80, 0x10000):
    c = chr(cp)
    if 0xD800 <= cp < 0xE000: continue
    try:
        c.encode('cp950'); chars.append(c)
    except UnicodeEncodeError:
        pass
chunks = [''.join(chars[i:i + 250]) for i in range(0, len(chars), 250)]
body = ',\n'.join('    "%s"' % ch for ch in chunks)
src = '''/*---
description: Big5（CP950）字表。遊戲內部是 UTF-8，要判斷一個字 zMUD 看不看得懂就查這張表。
note: 由 tools/gen_big5_table.py 產生，不要手改。共 %d 字。設計見 docs/EMBROIDERY.md。
---*/

private string *TABLE = ({
%s
});

// 這個字（UTF-8 的一個字元）Big5 有沒有；ASCII 一律視為有。
int has_char(string ch)
{
    string t;

    if( !stringp(ch) || !strlen(ch) ) return 0;
    if( strlen(ch) == 1 ) return 1;
    foreach(t in TABLE)
        if( strsrch(t, ch) >= 0 ) return 1;
    return 0;
}

// 整串文字的每個字都在 Big5 內才傳回 1。
int valid_text(string str)
{
    string ch;

    if( !stringp(str) ) return 0;
    foreach(ch in explode(str, ""))
        if( !has_char(ch) ) return 0;
    return 1;
}
''' % (len(chars), body)
open(OUT, 'w', encoding='utf-8', newline='\n').write(src)
print(f'{len(chars)} 字 -> {os.path.relpath(OUT, ROOT)}')
