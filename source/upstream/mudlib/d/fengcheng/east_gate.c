// /d/fengcheng/east_gate.c — 風城（L12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東城門");
    set("long", @LONG
城門洞高闊深邃，磚石被往來的車馬磨得發亮，守衛持戟分立兩側，盤查著進出的行旅。城外的山道在此分成兩股，一股往東隱入濃濃的霧氣，一股折向北邊的崖頂，走出城門，空氣陡然清冷起來。
LONG
    );
    set("exits", ([
        "west" : __DIR__"mountain_road",
        "south" : __DIR__"stone_road2",
        "northup" : __DIR__"cliff",
        "eastup" : "/d/tianshan/fog",
    ]));
    set("objects", ([
        "/obj/area/garrison" : 4,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
