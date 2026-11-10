// /d/snowkeep/side_n.c — 雪吟莊（J6）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "側廳");
    set("long", @LONG
側廳比正廳小了許多，陳設卻一樣講究，牆上掛著幾幅山水，案上一隻銅爐燃著檀香，煙縷細細地升上樑間。幾張花梨木椅沿牆排開，想是給赴會的賓客暫歇之用，廳東一道門簾半掩，後頭似乎是招待貴客的廂房。
LONG
    );
    set("exits", ([
        "east" : __DIR__"east_room",
        "south" : __DIR__"hall",
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
