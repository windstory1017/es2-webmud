// /d/swaylan/street2.c — 水嵐縣（Y20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
街邊一座磚砌高牆圍住了當鋪的後院，牆頭插著碎瓷片，門口有護衛把守，看來防範甚嚴。街上行人到了這裡腳步都快了幾分，偶有幾個衣衫寒酸的人在當鋪門前躊躇，終究還是走了進去。
LONG
    );
    set("exits", ([
        "west" : __DIR__"street1",
        "east" : __DIR__"alley",
        "north" : __DIR__"pawnshop",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
