// /d/tianshan/peach_grove.c — 天山（Z8）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "桃花林");
    set("long", @LONG
四周都是開滿桃花的桃樹，整個院子裡洋溢著桃花的香氣。地上長著一些五顏六色的不知名的植物。
LONG
    );
    set("exits", ([
        "west" : __DIR__"fork",
    ]));
    set("objects", ([
        "/custom/legacy_npc/mountain_god" : 1,
    ]));
    set("map/area", "天山");
    setup();
}
