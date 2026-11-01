// /d/swaylan/north_street1.c — 水嵐縣（U12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
這條街緊挨著北城門，進出城的行旅多半在此歇腳打尖，路邊擺著幾個茶水攤子，壺嘴冒著熱氣。往來的人群中不乏佩刀帶劍的江湖客，三三兩兩地站著說話，南腔北調，倒也熱鬧。
LONG
    );
    set("exits", ([
        "north" : __DIR__"north_gate",
        "south" : __DIR__"north_street2",
        "east" : __DIR__"dirt_road",
    ]));
    set("objects", ([
        __DIR__"npc/swordman" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
