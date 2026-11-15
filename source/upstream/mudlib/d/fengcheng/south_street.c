// /d/fengcheng/south_street.c — 風城（D18）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "南街");
    set("long", @LONG
這裡是風城比較繁榮的一條街。街的東邊有一個兩層的小客棧，客棧門口高掛著的燈籠上繡著春風兩個字。不少來遊天山的旅客都會在春風客棧住一兩天。
LONG
    );
    set("exits", ([
        "north" : __DIR__"crossroad",
        "south" : __DIR__"gate",
        "east" : __DIR__"inn",
    ]));
    set("objects", ([
        __DIR__"npc/seller" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
