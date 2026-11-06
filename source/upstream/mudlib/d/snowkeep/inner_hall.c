// /d/snowkeep/inner_hall.c — 雪吟莊（L8）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：舊資料（全文引用）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "雪吟莊大廳");
    set("long", @LONG
這裡是雪吟莊大廳，面向廳口的首位列著六張太師椅，顯然是預備給武林中威望最高的前輩「六賢」的座位，不過目前這些位置都是空著的。左右各八張較小的座位，座位旁都放著一個矮凳。
LONG
    );
    set("exits", ([
        "west" : __DIR__"hall",
    ]));
    set("objects", ([
        "/custom/legacy_npc/yu_tieshan" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
