// /d/swaylan/escort_gate.c — 水嵐縣（S20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "黃家鏢局");
    set("long", @LONG
黃家鏢局的大門前是一片寬敞的空地，地上鋪著平整的石板，足夠好幾輛鏢車同時停放。門楣上的金字匾額擦得發亮，兩旁旗桿上高懸著鏢旗，鏢師們進進出出，腳步沉穩，看得出鏢局生意興隆。
LONG
    );
    set("exits", ([
        "east" : __DIR__"south_street2",
        "west" : __DIR__"escort_hall",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
