// /d/baihua/school.c — 百花村（J12）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "學堂");
    set("long", @LONG
一間簡樸的學堂，屋裡擺著幾張矮桌，牆上掛著一幅寫著「勤學」的字。幾個小學童坐在桌前搖頭晃腦地念著書，眼睛卻不時偷偷瞄向窗外，似乎一心只想著出去玩耍。
LONG
    );
    set("exits", ([
        "east" : __DIR__"stone_road2",
    ]));
    set("objects", ([
        __DIR__"npc/student" : 3,
    ]));
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
