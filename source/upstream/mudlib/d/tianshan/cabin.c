// /d/tianshan/cabin.c — 天山（X2）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小木屋");
    set("long", @LONG
小木屋內擺設十分簡單，但是打掃的很乾淨。牆上掛著兩把鐵劍，中央供著神像。從房內的擺設來看，這一家應該是武術世家。
LONG
    );
    set("exits", ([
        "south" : __DIR__"dirt_road3",
    ]));
    set("objects", ([
        __DIR__"npc/liuh_qingqing" : 1,
    ]));
    set("map/area", "天山");
    setup();
}
