// /d/swaylan/buxuan/steps1.c — 步玄派（AM14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石階");
    set("long", @LONG
一道青石台階依著山勢層層而上，石縫中長滿了青苔，階旁種著幾叢修竹，竹葉上凝著細細的水珠。山間雲氣低低地貼著台階流過，走在其中，衣襬都沾上了幾分濕潤。
LONG
    );
    set("exits", ([
        "west" : __DIR__"gate",
        "east" : __DIR__"terrace",
        "south" : __DIR__"pond",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
