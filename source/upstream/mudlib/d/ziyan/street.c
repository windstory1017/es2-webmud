// /d/ziyan/street.c — 紫煙鎮（K8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "紫煙鎮大街");
    set("long", @LONG
紫煙鎮大街，說是大街，其實也不過兩輛車寬。路邊的店鋪有一半關著門，門板上落滿了灰塵，開著的幾家也冷冷清清，掌櫃倚在門邊打盹，整條街靜得只聽得見遠處羿水的流聲。
LONG
    );
    set("exits", ([
        "east" : __DIR__"well",
        "west" : __DIR__"narrow_street",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
