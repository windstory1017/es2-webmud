// /d/snowkeep/east_room.c — 雪吟莊（L6）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：舊資料（全文引用）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東廂房");
    set("long", @LONG
這裡是雪吟莊內的一間廂房，裝璜古朴典雅且有一絲尊貴，仿彿是為某位貴賓準備的，所以自然是打掃得一塵不染。
LONG
    );
    set("exits", ([
        "west" : __DIR__"side_n",
    ]));
    set("objects", ([
        "/custom/legacy_npc/ku_wu_wang" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
