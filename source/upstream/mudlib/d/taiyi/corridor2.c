// /d/taiyi/corridor2.c — 觀內走道（H12）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "觀內走道");
    set("long", @LONG
走道細長而安靜，簷下掛著一排竹編的燈籠，燈穗隨著氣流輕輕擺動。兩旁的房門都漆成了暗紅色，門上的銅環擦得發亮，想必每日都有人細心打理，連一絲灰塵也不見。
LONG
    );
    set("exits", ([
        "south" : __DIR__"corridor1",
        "north" : __DIR__"corridor3",
        "west" : __DIR__"guest2",
    ]));
    create_door("west", "廂房木門", "east", DOOR_CLOSED);
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
