// /d/fengcheng/street1.c — 風城（F16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
街旁店舖一家挨著一家，賣的多是山貨與乾糧，簷下掛滿了風乾的臘肉與草藥，氣味雜陳。遊山的旅客三三兩兩逛著，挑選要帶上山的物件，掌櫃們個個笑臉相迎，吆喝聲此起彼落。
LONG
    );
    set("exits", ([
        "west" : __DIR__"crossroad",
        "east" : __DIR__"street2",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
