// /d/shuiyue/woods.c — 水月村（F22）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "樹林");
    set("long", @LONG
密密的樹林裡光影斑駁，一條被踩實的小徑通向東邊，盡頭隱約露出一間木屋的屋角，屋簷下晾著幾張獸皮。林中除了遠處瀑布的轟鳴，便只有枝頭鳥雀偶爾的啼叫，與瀑布一帶的喧譁相比，這裡格外清幽。
LONG
    );
    set("exits", ([
        "north" : __DIR__"waterfall",
        "south" : __DIR__"sky_tree",
        "east" : __DIR__"hunter_hut",
    ]));
    set("map/area", "水月村");
    setup();
    create_door("east", "木門", "west", DOOR_CLOSED);
    replace_program(ROOM);
}
