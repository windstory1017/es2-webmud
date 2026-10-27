// /d/baihua/wang_home.c — 百花村（N12）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "老王的家");
    set("long", @LONG
老王是這個小村裡最德高望重的人了, 從有這個村開始, 老王就在這住下了. 他正坐在家門口向你搖著手, 示意你過去聊聊天, 和藹的笑容令你不自覺的想走了過去.
LONG
    );
    set("exits", ([
        "west" : __DIR__"stone_road2",
    ]));
    set("objects", ([
        __DIR__"npc/old_wang" : 1,
        __DIR__"npc/shepherd" : 1,
    ]));
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
