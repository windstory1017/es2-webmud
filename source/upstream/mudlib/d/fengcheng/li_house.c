// /d/fengcheng/li_house.c — 風城（F14）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "民宅");
    set("long", @LONG
這裡就是風城名士李儒家了，屋內充滿了草藥的味道。想來李老先生的身體是一天不如一天了。房間的擺設十分簡樸，都是些普通農家的擺設。
LONG
    );
    set("exits", ([
        "west" : __DIR__"stone_road1",
    ]));
    set("objects", ([
        "/custom/legacy_npc/li_ru" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
