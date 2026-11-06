// /d/snowkeep/dungeon_e.c — 雪吟莊（J2）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "地牢深處");
    set("long", @LONG
這是地牢最深處的一間石牢，三面都是堅硬的山岩，一面是手臂粗的鐵條。石牢裏收拾得意外乾淨，角落擺著一張石床，牆上卻刻滿了密密麻麻的劃痕，看來被關在這裡的人，已在暗無天日之中熬過了不少日子。
LONG
    );
    set("exits", ([
        "west" : __DIR__"dungeon",
    ]));
    set("objects", ([
        "/custom/legacy_npc/yu_lan_jian_yi" : 1,
    ]));
    set("map/area", "雪吟莊");
    set("map/layer", "地下");
    setup();
    replace_program(ROOM);
}
