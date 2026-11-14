// /d/fengcheng/stone_road1.c — 風城（D14）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石板路");
    set("long", @LONG
石板路自北面鋪來，兩旁店舖的幌子低垂，行人往來不絕。路面被磨出一道道淺淺的車轍，偶爾有挑著擔子的小販高聲吆喝，叫賣聲與馬蹄聲混作一片，透著小城特有的煙火氣。
LONG
    );
    set("exits", ([
        "south" : __DIR__"crossroad",
        "east" : __DIR__"li_house",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
