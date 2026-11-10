// /d/taiyi/hall.c — 太乙觀前廳（J14）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "太乙觀前廳");
    set("long", @LONG
跨過高高的門檻便是前廳，青石鋪地，四壁掛著幾幅褪了色的雲水圖。廳中擺著功德箱與簽筒，進香的人三三兩兩，低聲交談，腳步放得極輕。檀香的氣味沉沉地浮在樑下，怎麼也散不去。
LONG
    );
    set("exits", ([
        "southeast" : __DIR__"gate",
        "west" : __DIR__"corridor1",
        "east" : __DIR__"main_hall",
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
