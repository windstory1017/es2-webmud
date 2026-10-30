// /d/swaylan/forest_edge.c — 水嵐縣（AE14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林邊緣");
    set("long", @LONG
一片繁茂的密林在此展開，古木參天，枝葉交纏得密不透風，林中聽不見半聲鳥叫，靜得教人心裡發毛。林緣的灌木叢有一處被撥開了縫隙，斷枝上掛著幾縷破布，像是常有人從這裡鑽進鑽出。
LONG
    );
    set("exits", ([
        "southwest" : __DIR__"east_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
