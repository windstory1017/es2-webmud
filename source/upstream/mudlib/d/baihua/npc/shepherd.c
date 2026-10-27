// /d/baihua/npc/shepherd.c — 百花村 NPC：牧童（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("牧童", ({ "boy", "shepherd" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 12);
    set("long", @LONG
年輕的小童，每日照看牲畜很是悠哉。
LONG
    );
    set_power("C");
    setup();
    carry_object("/custom/weapon/staff/shepherd_cane")->wield();
}
