// /d/taiyi/corridor1.c — 觀內走道（H14）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "觀內走道");
    set("long", @LONG
走道兩側是粉牆黛瓦，地面鋪著被鞋底磨得發亮的青石板。一扇扇木門都關著，門縫裏漏出低低的談笑與翻身聲，想來是遠道而來投宿的香客，連廊下的腳步也不由得放輕了。
LONG
    );
    set("exits", ([
        "east" : __DIR__"hall",
        "north" : __DIR__"corridor2",
        "west" : __DIR__"guest3",
    ]));
    create_door("west", "廂房木門", "east", DOOR_CLOSED);
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
