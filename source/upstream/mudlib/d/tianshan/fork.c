// /d/tianshan/fork.c — 天山（X8）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "岔路口");
    set("long", @LONG
北邊有一個小木屋，不知道木屋的主人是誰。南面是一個小菜園，東面是一片桃林。
LONG
    );
    set("exits", ([
        "north" : __DIR__"dirt_road2",
        "west" : __DIR__"dirt_road1",
        "south" : __DIR__"garden",
        "east" : __DIR__"peach_grove",
    ]));
    set("map/area", "天山");
    setup();
}
