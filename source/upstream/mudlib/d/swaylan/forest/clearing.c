// /d/swaylan/forest/clearing.c — 密林（AI25）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "空地");
    set("long", @LONG
林間一片開闊空地，四周搭著幾座簡陋木棚，棚下堆著搶來的箱籠、布匹，亂七八糟地扔在一起。空地中央豎著一根木樁，樁上綁過人的繩索還沒解下，地上一灘發黑的血跡，不知是誰留下的。
LONG
    );
    set("exits", ([
        "north" : __DIR__"stele",
        "east" : __DIR__"kitchen",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 2,
        __DIR__"npc/bandit_leader" : 1,
    ]));
    set("map/area", "密林");
    setup();
    replace_program(ROOM);
}
