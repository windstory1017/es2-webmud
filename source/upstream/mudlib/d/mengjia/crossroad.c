// /d/mengjia/crossroad.c — 孟家莊（D12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "十字路口");
    set("long", @LONG
村子的十字路口，路心鋪著青石，被往來的車輪碾出淺淺的轍印。西邊的朱門高牆便是孟家，北邊是一間不大的小客棧，東邊的大街上人聲嘈雜，南來北往的行旅在此歇腳問路，又匆匆各奔前程。
LONG
    );
    set("exits", ([
        "north" : __DIR__"inn",
        "south" : "/d/fengcheng/stone_road1",
        "west" : __DIR__"gate",
        "east" : __DIR__"street",
    ]));
    set("map/area", "孟家莊");
    setup();
    replace_program(ROOM);
}
