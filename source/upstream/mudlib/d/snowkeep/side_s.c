// /d/snowkeep/side_s.c — 雪吟莊（J10）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "側廳");
    set("long", @LONG
這間側廳挨著內院，窗櫺雕著雲紋，透進來的光被切成一格一格。廳中一張長案上整整齊齊地疊著拜帖與名冊，硯臺裏的墨還沒乾透，看來莊上近來賓客極多，連帳房都忙不過來。
LONG
    );
    set("exits", ([
        "north" : __DIR__"hall",
        "east" : __DIR__"court",
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
