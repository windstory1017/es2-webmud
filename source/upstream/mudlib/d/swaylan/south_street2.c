// /d/swaylan/south_street2.c — 水嵐縣（U20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "南大街");
    set("long", @LONG
南大街到這裡變得寬敞，街西立著一座氣派的門樓，黑漆大門前蹲著兩頭石獅，門楣上懸著黃家鏢局的金字匾額。門前常有鏢車進出，車轍在青石板上碾出兩道深痕，一路通到大門口。
LONG
    );
    set("exits", ([
        "north" : __DIR__"south_street1",
        "south" : __DIR__"south_street3",
        "west" : __DIR__"escort_gate",
        "east" : __DIR__"street1",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
