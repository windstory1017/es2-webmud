// /d/baihua/temple.c — 百花村（J18）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "破舊小廟");
    set("long", @LONG
一座破舊的小廟，屋瓦殘缺不全，梁柱上的紅漆早已斑駁剝落。神案上的香爐積滿了灰，看來已經許久沒有人來上香了。廟門口蹲著一尊石獅子，雖然飽經風霜，神態依然威武。
LONG
    );
    set("exits", ([
        "northeast" : __DIR__"road2",
    ]));
    set("objects", ([
        __DIR__"npc/stone_lion" : 1,
    ]));
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
