// /d/swaylan/npc/woman.c — 水嵐縣 NPC：婦人（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("婦人", ({ "woman" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "female");
    set("age", 35);
    set("long", @LONG
一位提著菜籃的婦人，衣著樸素整潔，臉上帶著和氣的笑容，正忙著張羅家裡的飯菜。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    set_power("C");
    setup();
}
