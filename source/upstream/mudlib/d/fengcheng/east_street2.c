// /d/fengcheng/east_street2.c — 風城（L16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東街");
    set("long", @LONG
東街的盡頭靠近城牆，牆根下長著幾叢野草，風從城外灌進來，帶著松針與泥土的氣味。一條窄巷從牆根下蜿蜒向南，巷口的門板斑駁掉漆，看得出已有些年頭，似乎許久沒人修補過了。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_street1",
        "north" : __DIR__"stone_road2",
        "south" : __DIR__"alley",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
