// /d/ziyan/dirt_road.c — 紫煙鎮（G6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "泥土路");
    set("long", @LONG
通往渡口的一條泥土路，被往來的車馬輾得坑坑窪窪。路邊的水溝裡積著發黑的污水，散發出一股腥臭，偶有挑著魚簍的漁夫經過，腳步匆匆，濺起一路泥點，誰也不想在這裡多待。
LONG
    );
    set("exits", ([
        "north" : __DIR__"ferry",
        "south" : __DIR__"yamen_street",
        "west" : __DIR__"drugstore",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
