// /d/mengjia/tomb.c — 孟家莊（F6）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "墓室");
    set("long", @LONG
墓室裡冷得要命，而且黑糊糊的什麼也看不清楚。憑藉著上面透過來的微弱的光芒，勉強看得見一個箱子和一個棺材。
LONG
    );
    set("exits", ([
        "up" : __DIR__"graveyard",
    ]));
    set("objects", ([
        __DIR__"npc/tomb_guard" : 1,
        __DIR__"obj/chest" : 1,
    ]));
    set("map/area", "孟家莊");
    set("map/layer", "地下");
    setup();
    replace_program(ROOM);
}
