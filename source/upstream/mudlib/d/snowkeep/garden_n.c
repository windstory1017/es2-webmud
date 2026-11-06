// /d/snowkeep/garden_n.c — 雪吟莊（H4）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：舊資料（全文引用）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後花園");
    set("long", @LONG
這裡是雪吟山莊庭園中的一角，四週種滿了芍藥和牡丹，在風中爭芳鬥艷。一條鵝卵石鋪成的小路通像南邊的亭子，四週的青草也讓整個院子顯出一種和諧的高雅氣息。
LONG
    );
    set("exits", ([
        "south" : __DIR__"garden_s",
    ]));
    set("objects", ([
        "/custom/legacy_npc/yu_pingyin" : 1,
        "/d/snow/npc/guard" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
