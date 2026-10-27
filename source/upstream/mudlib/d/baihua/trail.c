// /d/baihua/trail.c — 百花村（J14）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "荒野小徑");
    set("long", @LONG
終於隱隱約約發現有條小徑呈現著東南, 西北的走向, 遠望過去似乎有個村落在東邊, 還好沒迷了路。
LONG
    );
    set("exits", ([
        "northwest" : __DIR__"hillside",
        "east" : __DIR__"road1",
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
