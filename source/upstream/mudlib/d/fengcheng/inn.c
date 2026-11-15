// /d/fengcheng/inn.c — 風城（F18）。

#include <room.h>

inherit INN;

void create()
{
    set("short", "春風客棧");
    set("long", @LONG
店面不大但是整理得乾乾淨淨。由於天山附近只有這一家客棧，所以生意興隆，春風得意。正好應了店名中的春風二字。
LONG
    );
    set("exits", ([
        "west" : __DIR__"south_street",
        "up" : __DIR__"inn2",
    ]));
    set("objects", ([
        __DIR__"npc/waiter" : 1,
    ]));
    set("map/area", "風城");
    setup();
}
