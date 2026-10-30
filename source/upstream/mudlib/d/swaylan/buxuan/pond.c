// /d/swaylan/buxuan/pond.c — 步玄派（AM16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "聚雲池");
    set("long", @LONG
半山腰上的一方池塘，池水清澈見底，終年雲氣氤氳，水面像是籠著一層薄紗。池中游魚成群，時而浮上水面啄食落葉，激起一圈圈漣漪，池邊的大石被坐得光滑，想必常有人在此靜坐。
LONG
    );
    set("exits", ([
        "north" : __DIR__"steps1",
    ]));
    set("map/area", "步玄派");
    setup();
    replace_program(ROOM);
}
