// /d/snowkeep/path.c — 雪吟莊（F6）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小徑");
    set("long", @LONG
一條碎石小徑在花木間曲曲折折地穿過，兩旁的冬青修剪得整整齊齊，有如兩道矮牆。腳下石子被踩得圓滑，走起來沙沙作響，四下裏只聞得到泥土與青草的氣息，與前院的熱鬧判若兩地。
LONG
    );
    set("exits", ([
        "south" : __DIR__"yard_w",
        "north" : __DIR__"pavilion",
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
