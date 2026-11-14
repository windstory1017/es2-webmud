// /d/shuiyue/tree_hollow.c — 水月村（J24）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "樹洞");
    set("long", @LONG
樹洞是被歲月蛀空的樹心，大小足夠容身，壁上留著一道道深深的抓痕，混著濃濃的獸腥味與乾草的氣息。洞底散落著零星的碎骨，暗處一雙雙冷光閃動，彷彿隨時會撲將上來。
LONG
    );
    set("exits", ([
        "out" : __DIR__"tree_top",
    ]));
    set("objects", ([
        __DIR__"npc/leopard" : 4,
    ]));
    set("map/area", "水月村");
    set("map/layer", "樹上");
    setup();
    replace_program(ROOM);
}
