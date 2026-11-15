// /d/fengcheng/tower.c — 風城（B20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "城樓");
    set("long", @LONG
城樓巍然而立，女牆後的垛口一個挨著一個，瞭望兵的長槍在雲霧中反著冷光。從這裡望出去，山下的村落與蜿蜒的官道盡收眼底，風聲呼呼灌進樓中，教人胸懷為之一闊。
LONG
    );
    set("exits", ([
        "down" : __DIR__"gate",
    ]));
    set("objects", ([
        "/custom/legacy_npc/si_ma_wei" : 1,
        "/obj/area/garrison" : 4,
    ]));
    set("map/area", "風城");
    set("map/layer", "城樓");
    setup();
    replace_program(ROOM);
}
