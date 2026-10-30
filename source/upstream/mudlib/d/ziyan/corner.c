// /d/ziyan/corner.c — 紫煙鎮（O8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街角");
    set("long", @LONG
街角這裡開著一間當鋪，門前常有人進進出出，進去時懷裡揣著包袱，出來時手裡多了幾串銅錢，臉上卻沒有半點喜色。牆上被人用炭寫了幾行字，罵的是縣太爺，字跡已經被擦得模糊不清。
LONG
    );
    set("exits", ([
        "north" : __DIR__"pawnshop",
        "south" : __DIR__"lane",
        "west" : __DIR__"well",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
