// /d/shuiyue/mud_road.c — 水月村（D24）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "泥石路");
    set("long", @LONG
路面由碎石與黃泥夯成，車轍深深淺淺，踩上去微微發軟。路旁幾株老樹的枝椏高高伸向天空，其中最粗壯的一株就在東邊不遠，樹影遮住了半條路，讓人不自覺放慢了腳步。
LONG
    );
    set("exits", ([
        "north" : __DIR__"stone_road",
        "south" : __DIR__"village1",
        "east" : __DIR__"sky_tree",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
