// /d/baihua/tile_house.c — 百花村（J10）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "瓦屋");
    set("long", @LONG
外觀破舊的瓦屋, 裡面卻相當清淨. 有股濃郁的藥材味撲鼻而來, 地上有多種材木, 味道顯然是由那些花花草草而來.
LONG
    );
    set("exits", ([
        "west" : __DIR__"cliff",
        "east" : __DIR__"stone_road1",
    ]));
    set("objects", ([
        __DIR__"npc/wang_yean" : 1,
    ]));
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
