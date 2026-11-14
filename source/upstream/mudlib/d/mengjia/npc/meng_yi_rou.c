// /d/mengjia/npc/meng_yi_rou.c — 孟家莊 NPC：孟憶柔（B8 後院）。

#include <npc.h>

inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("孟憶柔", ({ "meng yi-rou", "meng", "yi-rou" }));
    set_race("human");
    set_class("alchemist");
    set_level(10);
    set("gender", "female");
    set("age", 18);
    set("long", "孟家的掌上明珠，受兩位哥哥百般照顧，眉眼間還帶著幾分未脫的稚氣。近日被田由纏上，神色間總有些煩惱。\n");
    set_skill("unarmed", 20);
    set_skill("dodge", 30);
    set_skill("parry", 20);
    set_power("B");
    setup();
}
