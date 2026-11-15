// /d/fengcheng/street2.c — 風城（H16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
街心的青石板被往來車輪碾出兩道淺溝，一邊是將軍府高聳的圍牆，一邊是低矮的店舖，對比分明。圍牆內隱約傳來操練的號令，街上的行人走到這裡，不自覺都放輕了腳步。
LONG
    );
    set("exits", ([
        "west" : __DIR__"street1",
        "east" : __DIR__"east_street1",
        "north" : __DIR__"general_gate",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
