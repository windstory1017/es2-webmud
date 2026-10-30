// /d/swaylan/inn.c — 水嵐縣（U16）。房間敘述照設計表原文。

#include <room.h>

inherit INN;

void create()
{
    set("short", "五陵客棧");
    set("long", @LONG
你現在位於水嵐縣中最出名的五陵客棧前面，在各地往來的商旅，通常都會在這一家客棧休憩，在客棧的廂房中，無時無刻都有許多的旅人住在裡面，很多的人都會在這裡交換旅遊的心得，也有許多有關武林的傳聞也可以在這裡問到。在門的上頭掛著一幅匾額，上面提著【五陵客棧】四個大字。
LONG
    );
    set("exits", ([
        "west" : __DIR__"west_street",
        "east" : __DIR__"east_street1",
        "north" : __DIR__"north_street2",
        "south" : __DIR__"south_street1",
        "up" : __DIR__"inn2",
    ]));
    set("objects", ([
        __DIR__"npc/waiter" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
}
