// /d/qiaoyin_east/ground_e.c — 白雲觀（H2）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "練武場");
    set("long", @LONG
場中三具石假人排成一列，石面早被磨去了稜角，只剩模糊的人形輪廓。牆邊的兵器架上插著幾把木劍，劍柄纏著的粗布被汗水浸成了深色，角落裏的石鎖大小不一，想是讓人循序漸進地練氣力。
LONG
    );
    set("exits", ([
        "west" : __DIR__"yard",
        "south" : __DIR__"corridor_e",
    ]));
    set("objects", ([
        __DIR__"npc/stone_stake" : 3,
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
