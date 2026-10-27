// /d/baihua/platform.c — 百花村（H8）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "懸崖平台");
    set("long", @LONG
呼..呼....陣陣的山風颳在你身上，你遠望著三邊無際的景色，幾片雲絲飄浮在眼前，似乎伸手可及。這個平台相當隱密，有如仙界一般幽雅，令人心曠神怡，峭壁上的花朵，似乎有人在細心照料，開得十分繁盛。
LONG
    );
    set("exits", ([
        "eastup" : __DIR__"cliff",
    ]));
    set("objects", ([
        __DIR__"npc/kao_yeu" : 1,
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
