// /d/snowkeep/npc/blademan.c — 雪吟莊 NPC：雪吟莊刀客（強度比照冷梅莊二代弟子）。
// 設計見 docs/martial_arts/雪吟莊_武功.md 第七節。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("雪吟莊刀客", ({ "apprentice", "blademan" }));
    set_race("human");
    set_class("fighter");
    set_level(25);
    set("age", 25);
    set("long", @LONG
雪吟莊門下的刀客﹐刀法、內功都已有相當火候﹐常奉莊主之命在外行走。
LONG
    );
    set_skill("unarmed", 50);
    set_skill("dodge", 50);
    set_skill("parry", 50);
    set("custom_faction", "fighter.snow");   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("B");
    setup();
    carry_object("/obj/area/obj/blade")->wield();
    carry_object("/obj/area/obj/cloth")->wear();
}
