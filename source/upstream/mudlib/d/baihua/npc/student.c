// /d/baihua/npc/student.c — 百花村 NPC：小學童（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("小學童", ({ "student", "students" }));
    set_race("human");
    set_class("commoner");
    set_level(3);
    set("gender", "male");
    set("age", 5);
    set("long", @LONG
跟著先生念詩的小學童，心不在焉的想跑出去玩。
LONG
    );
    set_power("C");
    setup();
}
