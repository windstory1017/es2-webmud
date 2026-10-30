// /d/swaylan/buxuan/field1.c — 步玄派（AO16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "農田");
    set("long", @LONG
步玄派的田地沿著山坡開墾成一層層的梯田，田埂修得筆直，稻禾青青，長得十分茁壯。田邊立著一座簡陋的草棚，棚下放著鋤頭、竹簍等農具，派中弟子耕讀並重，連種田也毫不馬虎。
LONG
    );
    set("exits", ([
        "north" : __DIR__"terrace",
        "east" : __DIR__"field2",
    ]));
    set("objects", ([
        __DIR__"npc/farmer" : 2,
    ]));
    set("map/area", "步玄派");
    setup();
    replace_program(ROOM);
}
