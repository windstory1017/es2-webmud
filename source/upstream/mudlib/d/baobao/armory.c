// /d/baobao/armory.c — 八寶樓（D10）。敘述：新寫（設計表：敘述你寫）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "兵器庫");
    set("long", @LONG
石室窄而深長，四壁釘滿鐵鉤與木架，長槍大戟一排排斜倚著，槍纓早已褪成暗褐。空氣裡滿是桐油與鐵鏽的氣味，石板地上留著拖曳兵刃磨出的淺痕，一角有道陡窄的木梯通往樓上。
LONG
    );
    set("exits", ([
        "up" : __DIR__"forge",
    ]));
    set("objects", ([
        "/custom/legacy_npc/si_ma_lei" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
