// /d/baobao/study.c — 八寶樓（F10）。敘述：舊資料（全文）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "歐陽書房");
    set("long", @LONG
屋內洋溢著一股醉人的墨香味，書房四周的牆上挂著幾張狂草。書桌上擺著一幅剛剛寫好的對聯。看起來這裡的主人文學功底頗深。
LONG
    );
    set("exits", ([
        "south" : "/d/fengcheng/inner_yard",
        "west" : __DIR__"armory",
    ]));
    set("objects", ([
        "/custom/legacy_npc/o_yan_zhi" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
