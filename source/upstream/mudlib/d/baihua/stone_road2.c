// /d/baihua/stone_road2.c — 百花村（L12）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石子路");
    set("long", @LONG
百花村裡的石子路，路面打掃得乾乾淨淨，看不到一片落葉。路邊的竹籬笆上攀滿了牽牛花，幾隻蝴蝶在花叢間翩翩飛舞，偶爾有村民挑著花籃經過，朝你和善地笑一笑。
LONG
    );
    set("exits", ([
        "north" : __DIR__"stone_road1",
        "west" : __DIR__"school",
        "east" : __DIR__"wang_home",
        "south" : __DIR__"road1",
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
