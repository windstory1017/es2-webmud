// /d/ziyan/hut.c — 紫煙鎮（I6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "茅屋");
    set("long", @LONG
一間低矮的茅屋，屋頂的茅草塌了一角，屋裡昏暗潮濕，只擺著一張破木床和一張缺腿的桌子。地上的泥土有幾處顏色特別深，像是被什麼東西浸透過，牆角堆著一捆草蓆，透著一股說不出的腥味。
LONG
    );
    set("exits", ([
        "out" : __DIR__"narrow_street",
    ]));
    set("objects", ([
        "/custom/legacy_npc/cheng_huai" : 1,
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
