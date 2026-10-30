// /d/swaylan/woods2.c — 水嵐縣（Q10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "樹林");
    set("long", @LONG
林子越往裡走越密，粗大的樹幹上留著一道道斧痕，地上散落著劈下的枝椏和木屑，空氣裡滿是松脂的氣味。篤篤的伐木聲在林間來回迴盪，一聲接著一聲，聽來格外清晰。
LONG
    );
    set("exits", ([
        "south" : __DIR__"woods1",
    ]));
    set("objects", ([
        __DIR__"npc/cutter" : 2,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
