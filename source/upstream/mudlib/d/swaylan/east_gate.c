// /d/swaylan/east_gate.c — 水嵐縣（AC16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東城門");
    set("long", @LONG
東城門外便是起伏的丘陵，城樓上插著幾面旗幟，守衛把守得頗為嚴密，出入的行人都要被盤問幾句。城牆東北角外隱約可見一片黑壓壓的密林，城裡人說起那片林子，總要壓低聲音。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_street3",
        "east" : __DIR__"hillside",
        "south" : __DIR__"street_se",
        "northeast" : __DIR__"forest_edge",
    ]));
    set("objects", ([
        __DIR__"npc/gate_guard" : 4,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
