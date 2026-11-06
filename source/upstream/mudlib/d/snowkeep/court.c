// /d/snowkeep/court.c — 雪吟莊（L10）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：舊資料（全文引用）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "天井");
    set("long", @LONG
這裡是雪吟莊側廳的一處天井﹐北邊是通往正廳的走廊﹐東邊與南邊都是廂房﹐西邊則是側廳﹐天井中放著幾盆發著淡淡幽香的蘭花﹐地上整齊地鋪著灰石磚。
LONG
    );
    set("exits", ([
        "west" : __DIR__"side_s",
    ]));
    set("objects", ([
        "/custom/legacy_npc/phoxin" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
