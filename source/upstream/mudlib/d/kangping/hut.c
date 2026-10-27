// /d/kangping/hut.c — 康平村（J6）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "草廬");
    set("long", @LONG
你現在來到一間草廬之中，廬中的桌椅傢俱都相當粗陋，有些還是帶著樹皮的木頭做的，一邊的牆上掛著幾件簑衣，門邊放著一個小火爐，從窗戶望出去可以看到草廬後有個小池塘。
LONG
    );
    set("exits", ([
        "west" : __DIR__"square",
    ]));
    set("objects", ([
        "/custom/legacy_npc/old_taoist" : 1,
    ]));
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
