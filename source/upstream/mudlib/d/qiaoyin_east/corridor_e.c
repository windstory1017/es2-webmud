// /d/qiaoyin_east/corridor_e.c — 白雲觀（H4）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東廊");
    set("long", @LONG
簷下掛著一排銅鈴，鈴舌靜靜垂著，積了薄薄一層灰。廊邊的長凳被坐得溫潤，靠柱處擺著一盆養得極好的菊花。牆那一頭的練武場人聲不斷，這裡卻只聽得見自己的腳步聲。
LONG
    );
    set("exits", ([
        "north" : __DIR__"ground_e",
        "south" : __DIR__"refectory",
        "west" : __DIR__"court",
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
