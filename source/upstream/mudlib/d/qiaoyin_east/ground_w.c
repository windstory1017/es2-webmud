// /d/qiaoyin_east/ground_w.c — 白雲觀（D2）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "練武場");
    set("long", @LONG
青石鋪成的方場被踏得光滑發亮，場邊立著三具石雕的假人，肩頭與胸口佈滿深淺不一的拳痕與劍痕。牆角擱著幾把木劍與練功用的沙袋，磨得發白的石面上留著前人踏出的凹窩，一看便知常有人在此流汗。
LONG
    );
    set("exits", ([
        "east" : __DIR__"yard",
        "south" : __DIR__"corridor_w",
    ]));
    set("objects", ([
        __DIR__"npc/stone_stake" : 3,
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
