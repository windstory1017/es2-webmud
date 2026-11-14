// /d/fengcheng/crossroad.c — 風城（D16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "交叉路口");
    set("long", @LONG
這裡是風城的正中央，南面是春風客棧，北面是當地名士李儒的家，東邊是將軍府，將軍府再往東，出了東城門，就是天山了。
LONG
    );
    set("exits", ([
        "north" : __DIR__"stone_road1",
        "south" : __DIR__"south_street",
        "east" : __DIR__"street1",
    ]));
    set("objects", ([
        __DIR__"npc/chen_xue_liang" : 1,
        __DIR__"npc/fortune_teller" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
