// /d/swaylan/forest/entrance.c — 密林（AC23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林入口");
    set("long", @LONG
茂密的樹林從水嵐縣城外一路延伸上山，林木高大，枝葉遮天蔽日，林中幽暗得看不清路。樹幹上留著幾道陳舊的刀痕，地上散落著折斷的箭桿，據說官兵曾經多次進林掃蕩，卻都鎩羽而歸。
LONG
    );
    set("exits", ([
        "east" : __DIR__"forest1",
        "out" : "/d/swaylan/forest_edge",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 2,
    ]));
    set("map/area", "密林");
    setup();
    replace_program(ROOM);
}
