// /d/snowkeep/dungeon_w.c — 雪吟莊（F2）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "地牢");
    set("long", @LONG
石階盡頭竟是一間開鑿在山腹裏的石室，四壁濕冷，水珠沿著石縫一滴一滴落下，在寂靜中格外清晰。牆上插著一支將熄未熄的火把，照出一排生了鏽的鐵柵，誰也想不到名滿江湖的雪吟莊底下，竟藏著這樣一座地牢。
LONG
    );
    set("exits", ([
        "up" : __DIR__"pavilion",
        "east" : __DIR__"dungeon",
    ]));
    set("map/area", "雪吟莊");
    set("map/layer", "地下");
    setup();
    replace_program(ROOM);
}
