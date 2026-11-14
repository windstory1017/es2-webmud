// /d/shuiyue/hunter_hut.c — 水月村（H22）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "木屋內");
    set("long", @LONG
你走進了木屋, 桌旁坐了一個面帶愁容的老人, 牆上掛了許多的獵具, 也掛了許多猛獸的毛皮, 看來都是這個獵戶的戰利品.
LONG
    );
    set("exits", ([
        "west" : __DIR__"woods",
    ]));
    set("objects", ([
        __DIR__"npc/yu_tian_shan" : 1,
    ]));
    set("map/area", "水月村");
    setup();
    create_door("west", "木門", "east", DOOR_CLOSED);
    replace_program(ROOM);
}
