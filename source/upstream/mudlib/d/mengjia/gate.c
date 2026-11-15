// /d/mengjia/gate.c — 孟家莊（B12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "孟家大門");
    set("long", @LONG
孟家大門高大厚實，門前兩尊石獅被人摸得溫潤發亮，朱漆銅環上落著一層細細的塵。門額上一個蒼勁的「孟」字，來往的鄉人與江湖客走到這裡，總要多看兩眼，又不自覺把聲音壓低了幾分。
LONG
    );
    set("exits", ([
        "north" : __DIR__"manor",
        "south" : __DIR__"herb_shop",
        "east" : __DIR__"crossroad",
    ]));
    set("objects", ([
        "/custom/legacy_npc/tian_yu" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
    replace_program(ROOM);
}
