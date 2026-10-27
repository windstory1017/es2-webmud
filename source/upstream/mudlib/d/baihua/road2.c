// /d/baihua/road2.c — 百花村（L16）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "常春道");
    set("long", @LONG
你走在一條小石子滿地的路上, 兩旁的花草樹木十分整齊的排列著, 右邊紅色燈籠整齊的掛在屋簷下, 看起來應該是個客棧. 遠眺南方草木叢生, 似乎人煙罕至, 隱隱的有幾聲狼嗥傳來, 坡度向東南而下.
LONG
    );
    set("exits", ([
        "north" : __DIR__"road1",
        "east" : __DIR__"inn",
        "southwest" : __DIR__"temple",
    ]));
    set("objects", ([
        __DIR__"npc/nan_kong_hen" : 1,
        "/obj/area/man" : 1,
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
