// /d/taiyi/corridor3.c — 觀內走道（H10）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "觀內走道");
    set("long", @LONG
走道到了盡頭，一堵粉牆擋在眼前，牆根下的青苔爬了半人高，牆上砌著一方小小的花窗，窗外隱約是山上的樹影。此處遠離前殿的人聲，只剩風過竹葉的沙沙聲，一片清幽。
LONG
    );
    set("exits", ([
        "south" : __DIR__"corridor2",
        "west" : __DIR__"guest1",
        "northeast" : __DIR__"trail1",
    ]));
    create_door("west", "廂房木門", "east", DOOR_CLOSED);
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
