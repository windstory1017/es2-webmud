// /d/qiaoyin/pawnshop.c — 喬陰縣（J6）。依使用者設計表（2026-10-16）。

#include <room.h>

inherit HOCKSHOP;

void create()
{
    set("short", "和豐當鋪");
    set("long", @LONG
一間門面不大的當鋪，高高的櫃臺後面坐著朝奉，只露出一雙精明的眼睛，櫃臺前擺著一張長凳，供來典當的客人歇腳。牆上掛著一塊寫著「童叟無欺」的木牌，字跡已經有些模糊。
LONG
    );
    set("exits", ([
        "west" : __DIR__"mid_lane1",
    ]));
    set("objects", ([
        __DIR__"npc/hsiao_bi_chen" : 1,
        "/custom/legacy_npc/hsiao_lang_sheng" : 1,
    ]));
    set("map/area", "喬陰縣");
    setup();
}
