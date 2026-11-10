// /d/taiyi/passage2.c — 通道（L6）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "通道");
    set("long", @LONG
通道在此轉了個彎，石壁漸漸由人工砌築轉為天然岩層，縫隙間滲進絲絲涼意，隱約還帶著草木的氣息。地上的青磚磨損得厲害，凹凹凸凸的，顯然曾有人經常往返於此。
LONG
    );
    set("exits", ([
        "south" : __DIR__"passage1",
        "east" : __DIR__"passage3",
    ]));
    set("map/layer", "地下");
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
