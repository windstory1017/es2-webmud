// /d/tianshan/garden.c — 天山（X10）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小菜園");
    set("long", @LONG
一塊整整齊齊的小菜園，畦上種著青菜與蘿蔔，行距分明，顯然有人精心照料。園邊用竹竿搭起幾架豆棚，藤蔓爬得老高，泥土裡混著淡淡的桃花香，與山下的菜地全然不同。
LONG
    );
    set("exits", ([
        "north" : __DIR__"fork",
    ]));
    set("map/area", "天山");
    setup();
}
