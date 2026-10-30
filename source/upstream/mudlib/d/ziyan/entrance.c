// /d/ziyan/entrance.c — 紫煙鎮（O16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "紫煙鎮口");
    set("long", @LONG
紫煙鎮的鎮口，一條黃土大路從這裡往東伸展，通往繁華的水嵐縣。路旁立著一座斑駁的石牌坊，上頭「紫煙鎮」三個字已經剝落了大半，牌坊下積著厚厚的塵土，一旁的茶棚早已沒了生意。
LONG
    );
    set("exits", ([
        "north" : __DIR__"inn_street",
        "east" : "/d/swaylan/west_gate",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
