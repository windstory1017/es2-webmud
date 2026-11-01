// /d/swaylan/dirt_road.c — 水嵐縣（W12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "泥土路");
    set("long", @LONG
一條坑坑窪窪的泥土路，路面還留著當年車轍壓出的深溝，看得出這裡曾經車馬不絕。如今渡口移了位置，路邊的茶棚只剩幾根歪斜的竹竿，野草從車轍裡冒了出來，一片冷清。
LONG
    );
    set("exits", ([
        "north" : __DIR__"old_ferry",
        "west" : __DIR__"north_street1",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
