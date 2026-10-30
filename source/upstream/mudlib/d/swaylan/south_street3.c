// /d/swaylan/south_street3.c — 水嵐縣（U22）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "南大街");
    set("long", @LONG
南大街盡頭一帶，店鋪漸漸稀疏，路旁多了幾家車馬店，院子裡停著大車，拴著騾馬，草料與牲口的氣味撲鼻而來。趕車的腳夫圍在一起蹲著吃飯，說的盡是京畿一路上的見聞。
LONG
    );
    set("exits", ([
        "north" : __DIR__"south_street2",
        "south" : __DIR__"south_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
