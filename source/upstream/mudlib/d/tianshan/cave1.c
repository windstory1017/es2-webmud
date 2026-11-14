// /d/tianshan/cave1.c — 天山（P8）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山洞內");
    set("long", @LONG
山洞裡一片漆黑你什麼也看不見。
LONG
    );
    set("exits", ([
        "out" : __DIR__"spring",
        "east" : __DIR__"cave2",
    ]));
    set("map/area", "天山");
    set("map/layer", "洞穴");
    setup();
}
