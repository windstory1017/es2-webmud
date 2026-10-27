// /d/baihua/cliff_edge.c — 百花村（N10）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "峭壁邊");
    set("long", @LONG
峭壁的邊緣，一陣山風迎面吹來，吹得人衣袂翻飛。往下望去，雲霧在山腰間緩緩流動，底下的林木小得像一片青苔，遠方的山巒層層疊疊，直到天邊才與雲色融成一片。
LONG
    );
    set("exits", ([
        "west" : __DIR__"stone_road1",
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
