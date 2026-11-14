// /d/tianshan/dirt_road1.c — 天山（V8）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "土路");
    set("long", @LONG
一條崎嶇小土路向西邊延伸，微風中醉人的桃花香讓你浮想翩翩。你不禁懷疑到底是何方仙人隱居於此。
LONG
    );
    set("exits", ([
        "west" : __DIR__"cave_mouth",
        "east" : __DIR__"fork",
    ]));
    set("map/area", "天山");
    setup();
}
