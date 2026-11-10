// /d/taiyi/cell.c — 廂房（N14）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "廂房");
    set("long", @LONG
這是一間清靜的小廂房，窗下一張舊木榻，榻上放著蒲團，牆角的銅爐冒著細細的青煙。桌上攤著幾卷泛黃的道經，書頁邊緣被翻得起了毛，想來主人時常展讀，一室之內只聞淡淡的墨香。
LONG
    );
    set("exits", ([
        "west" : __DIR__"main_hall",
    ]));
    set("objects", ([
        __DIR__"npc/shang_ching" : 1,
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
