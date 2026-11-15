// /d/fengcheng/cliff.c — 風城（L10）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "天山孤崖");
    set("long", @LONG
高處不勝寒，剛一來到天山的頂峰，你立刻打了個哆嗦。向西邊望去，透過層層迷霧，那風城竟然也顯得神秘莫測。
LONG
    );
    set("exits", ([
        "southdown" : __DIR__"east_gate",
    ]));
    set("objects", ([
        "/obj/area/man" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
