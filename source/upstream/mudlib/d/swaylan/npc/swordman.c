// /d/swaylan/npc/swordman.c — 水嵐縣 NPC：劍客（依水嵐縣設計表）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("劍客", ({ "sword man", "swordman" }));
    set_race("human");
    set_class("fighter");
    set_level(10);
    set("age", 20);
    set("long", @LONG
一個背著長劍的年輕人，衣衫沾著些風塵，眉宇間帶著幾分傲氣，看樣子是出來闖蕩江湖的。
LONG
    );
    set_skill("unarmed", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_skill("sword", 20);
    set_power("C");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/obj/area/obj/shortsword");
}
