// /d/ziyan/inn.c — 紫煙鎮（K14）。房間敘述照設計表原文（舊站原文）

#include <room.h>

inherit INN;

void create()
{
    set("short", "紫煙小棧");
    set("long", @LONG
紫煙小棧是紫煙鎮上唯一的小客棧, 這店棧雖然在城外, 規模卻是不小, 一進門是一面櫃檯, 櫃檯前面, 卻放著十餘張椅子, 想必是借人歇腳用的, 你可以向店小二打聽一下店裡有賣哪些食物.
LONG
    );
    set("exits", ([
        "east" : __DIR__"inn_gate",
    ]));
    set("objects", ([
        __DIR__"npc/waiter" : 1,
    ]));
    set("map/area", "紫煙鎮");
    setup();
}
