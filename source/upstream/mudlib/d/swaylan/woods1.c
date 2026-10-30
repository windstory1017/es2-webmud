// /d/swaylan/woods1.c — 水嵐縣（Q12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "樹林");
    set("long", @LONG
城牆外的一片雜木林，林邊被踩出一條小徑，兩旁堆著捆好的柴薪，等著挑進城裡去賣。樹蔭底下透著一股潮濕的泥土味，偶爾有鳥雀從枝頭撲稜稜飛起，又落進更深的林子裡。
LONG
    );
    set("exits", ([
        "north" : __DIR__"woods2",
        "east" : __DIR__"crossroad",
    ]));
    set("objects", ([
        __DIR__"npc/cutter" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
