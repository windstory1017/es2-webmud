// /d/baihua/stone_road1.c — 百花村（L10）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石子路");
    set("long", @LONG
一條鋪滿碎石子的小路，石子被往來的腳步磨得圓潤光滑，走起來沙沙作響。路旁種著一排排花草，紅的、黃的、紫的開成一片，花香隨風飄散，難怪這裡被稱作百花村。
LONG
    );
    set("exits", ([
        "west" : __DIR__"tile_house",
        "east" : __DIR__"cliff_edge",
        "south" : __DIR__"stone_road2",
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
