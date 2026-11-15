// /d/shuiyue/stone_house.c — 水月村（J20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石屋");
    set("long", @LONG
石屋不大，卻收拾得井井有條，粗木桌椅與一口灶火一應俱全，頭頂開著一道天井，天光與山風一同灌進來，使屋裡既不陰濕也不悶熱。牆邊的劍架上擱著幾柄舊劍，劍身擦得雪亮，映出屋主人素日的性情。
LONG
    );
    set("exits", ([
        "out" : __DIR__"cave",
    ]));
    set("objects", ([
        __DIR__"npc/wei_feng" : 1,
    ]));
    set("map/area", "水月村");
    set("map/layer", "洞穴");
    setup();
    replace_program(ROOM);
}
