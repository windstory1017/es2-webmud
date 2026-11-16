// /d/qiaoyin_east/yard.c — 白雲觀（F2）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後院");
    set("long", @LONG
後院方方正正，院心一口古井，井欄以青石砌成，繩子在欄緣勒出一道深深的溝。牆根下種著幾叢翠竹與一畦藥草，泥土的濕氣混著竹葉的清香，水桶輕碰井壁的聲音在院裏格外分明。
LONG
    );
    set("exits", ([
        "west" : __DIR__"ground_w",
        "east" : __DIR__"ground_e",
        "south" : __DIR__"court",
    ]));
    set("objects", ([
        "/d/lee/obj/well" : 1,
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
