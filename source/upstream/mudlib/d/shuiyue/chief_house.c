// /d/shuiyue/chief_house.c — 水月村（B28）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "村長家");
    set("long", @LONG
屋子是村裡少見的磚瓦房，樑柱雖舊，卻擦得光亮。堂上掛著一幅泛黃的山水，下面一張八仙桌，桌上擺著賬冊與算盤，村裡大小事務想必都在此商議。門檻被來來往往的鄉親踩得凹下去一塊。
LONG
    );
    set("exits", ([
        "east" : __DIR__"crossroad",
    ]));
    set("objects", ([
        __DIR__"npc/old_tu" : 1,
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
