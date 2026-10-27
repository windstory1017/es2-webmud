// /d/baihua/npc/old_wang.c — 百花村 NPC：老王（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("老王", ({ "old wang", "wang" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 55);
    set("long", @LONG
百花村最德高望重的人，慈祥和藹的樣子背後藏著令人辛酸的往事。
LONG
    );
    set_power("C");
    setup();
}
