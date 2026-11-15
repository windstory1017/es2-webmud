// /d/tianshan/dirt_road2.c — 天山（X6）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "土路");
    set("long", @LONG
土路沿著山腰向北蜿蜒，兩旁的桃樹漸漸稀疏，換成一叢叢半人高的野草，腳底踩過去沙沙作響。路面上落著幾瓣不知從哪兒飄來的桃花瓣，被腳步帶得輕輕翻滾，四下裡靜得出奇。
LONG
    );
    set("exits", ([
        "north" : __DIR__"dirt_road3",
        "south" : __DIR__"fork",
    ]));
    set("map/area", "天山");
    setup();
}
