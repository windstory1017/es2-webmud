// /d/fengcheng/npc/waiter.c — 風城 NPC：店小二（F18 春風客棧）。

#include <npc.h>

inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("店小二", ({ "waiter" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "male");
    set("age", 21);
    set("long", "春風客棧的店小二，肩上搭著一條白毛巾，見人就笑，手腳勤快得很。\n");
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}
