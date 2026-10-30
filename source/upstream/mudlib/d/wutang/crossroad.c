// /d/wutang/crossroad.c — 五堂鎮（N12）。房間敘述照設計表原文。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "交叉路口");
    set("long", @LONG
這裡是五堂鎮廟口大街的一處交叉路口﹐廟口大街往東西延伸﹐路口中央立著一根高大的鐵旗桿﹐據說是鎮上雍家的先祖所立﹐幾百年來已成為五堂鎮的地標之一﹐街道往東西南北都相當熱鬧﹐人來人往﹐由於五堂鎮位當喬陰縣的水路要衝﹐因此這裡的繁華景象和縣城比起來是毫不遜色。
LONG
    );
    set("exits", ([
        "west" : __DIR__"west_street1",
        "east" : __DIR__"east_street1",
        "north" : __DIR__"north_street",
        "south" : __DIR__"south_street1",
    ]));
    set("objects", ([
        __DIR__"npc/seller" : 1,
        __DIR__"npc/guo_boo" : 1,
    ]));
    set("map/area", "五堂鎮");
    setup();
    replace_program(ROOM);
}
