// /d/swaylan/buxuan/gate.c — 步玄派（AK14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "步玄派");
    set("long", @LONG
步玄派的山門建在半山腰上，兩根石柱撐起一座樸素的牌坊，橫匾上刻著「步玄」二字，筆意灑脫。門內傳來朗朗的讀書聲，間或夾著劍刃破風之聲，耕讀習武，自給自足，倒也別有一番氣象。
LONG
    );
    set("exits", ([
        "southwest" : __DIR__"mountain_path",
        "east" : __DIR__"steps1",
    ]));
    set("objects", ([
        __DIR__"npc/apprentice" : 2,
    ]));
    set("map/area", "落雲峰");
    setup();
    replace_program(ROOM);
}
