// /d/swaylan/forest/clearing.c — 密林（AI25）。房間敘述依設計表需求撰寫；2026-11-01 改為地底的空地。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "空地");
    set("long", @LONG
地底下竟挖出了一片開闊的空地，頭頂用粗大的木樑撐著，泥土間垂下幾縷樹根。四周搭著幾座簡陋木棚，棚下堆著搶來的箱籠、布匹，中央豎著木樁，樁上綁過人的繩索還沒解下，地上一灘發黑的血跡。
LONG
    );
    set("exits", ([
        "up" : __DIR__"stele",
        "south" : __DIR__"tunnel",
        "east" : __DIR__"kitchen",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 2,
        __DIR__"npc/bandit_leader" : 1,
    ]));
    set("map/area", "密林");
    set("map/layer", "地下");   // 推石碑掉下來的地底（2026-11-01）
    setup();
    replace_program(ROOM);
}
