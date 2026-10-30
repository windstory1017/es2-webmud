// /d/swaylan/east_street1.c — 水嵐縣（W16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東大街");
    set("long", @LONG
東大街是水嵐縣最熱鬧的一段，青石板路寬得容得下兩輛馬車並行，兩旁酒樓茶肆林立，簷下掛著一串串燈籠。街角飄來剛出籠的包子香，混著酒館裡划拳的吆喝聲，熱熱鬧鬧地連成一片。
LONG
    );
    set("exits", ([
        "west" : __DIR__"inn",
        "east" : __DIR__"east_street2",
        "south" : __DIR__"chien_store",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
