// /d/kangping/square.c — 康平村（H6）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "廣場");
    set("long", @LONG
村子中央的一片空地，正中有一口古井，井邊的青石被打水的人磨得發亮。村民們常在這裡洗衣、打水、閒話家常，孩子們繞著井邊追逐嬉鬧，是全村上下最熱鬧的地方。
LONG
    );
    set("exits", ([
        "north" : __DIR__"valley_mouth",
        "east" : __DIR__"hut",
    ]));
    set("objects", ([
        __DIR__"npc/kid" : 1,
    ]));
    set("outdoors", "kangping");
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
