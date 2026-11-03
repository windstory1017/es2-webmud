// /d/wutang/pavilion.c — 五堂鎮（F12）。房間敘述依設計表需求擴寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "涼亭");
    set("long", @LONG
一座建在小丘上的木造涼亭，四周種著幾株垂柳，微風吹過時柳枝輕輕搖曳。站在亭中向西望去，可以眺望寬闊的羿水在遠方緩緩流過，河面上點點帆影，景色十分宜人。亭外便是一條碎石路。
LONG
    );
    set("exits", ([
        "out" : __DIR__"gravel_road_n",
    ]));
    // 楊子陵與冷梅莊二代弟子由碎石路移來（使用者指示 #26，2026-11-03）。
    set("objects", ([
        __DIR__"npc/yang_zlin" : 1,
        __DIR__"npc/apprentice" : 1,
    ]));
    set("map/area", "五堂鎮");
    setup();
    replace_program(ROOM);
}
