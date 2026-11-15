// /d/shuiyue/village1.c — 水月村（D26）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "水月村");
    set("long", @LONG
村口的老槐樹下擺著幾張竹凳，村民三三兩兩聚在一塊閒話家常，孩童追逐嬉笑，雞犬相聞，一派恬靜。只是不時有人抬頭望向天空，似乎對近年出沒的巨鳥仍有些放不下心。
LONG
    );
    set("exits", ([
        "north" : __DIR__"mud_road",
        "south" : __DIR__"crossroad",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
