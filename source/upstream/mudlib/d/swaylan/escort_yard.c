// /d/swaylan/escort_yard.c — 水嵐縣（O18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後院");
    set("long", @LONG
鏢局的後院種著幾株老槐樹，樹下擺著石鎖與木樁，是鏢師們平日練功的地方，地面被踩得結結實實。院子另一頭有一道月洞門，門後便是黃家人起居的內宅，鏢師守在門前，閒人不得擅入。
LONG
    );
    set("exits", ([
        "east" : __DIR__"escort_side",
        "south" : __DIR__"escort_back",
    ]));
    set("objects", ([
        __DIR__"npc/escort_guard" : 2,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
