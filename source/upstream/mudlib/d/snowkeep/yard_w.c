// /d/snowkeep/yard_w.c — 雪吟莊（F8）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "前院");
    set("long", @LONG
穿過大門便是前院，青石板鋪得平平整整，兩旁各植一排蒼勁的老松。院中立著幾座兵器架，刀槍劍戟擦得雪亮，地上留著不少深淺不一的腳印，想必莊中弟子平日便在這裡操練，空氣裏隱約還有一股汗水與鐵鏽的氣味。
LONG
    );
    set("exits", ([
        "west" : __DIR__"entrance",
        "east" : __DIR__"yard_e",
        "north" : __DIR__"path",
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
