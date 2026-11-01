// /d/swaylan/lunmay/npc/apprentice3.c — 冷梅莊 NPC：冷梅莊三代弟子（依水嵐縣設計表）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("冷梅莊三代弟子", ({ "apprentice" }));
    set_race("human");
    set_class("fighter");
    set_level(10);
    set("age", 15);
    set("long", @LONG
冷梅莊弟子，在劍法上稍有成就，尚未涉足更深入的武學。
LONG
    );
    set_skill("unarmed", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_skill("sword", 20);
    set("custom_faction", "fighter.lunmay");   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("C");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/obj/area/obj/cloth")->wear();
}
