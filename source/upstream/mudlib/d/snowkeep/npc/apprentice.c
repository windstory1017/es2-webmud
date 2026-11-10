// /d/snowkeep/npc/apprentice.c — 雪吟莊 NPC：雪吟莊弟子（強度比照冷梅莊三代弟子）。
// 設計見 docs/martial_arts/雪吟莊_武功.md 第七節。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("雪吟莊弟子", ({ "apprentice" }));
    set_race("human");
    set_class("fighter");
    set_level(10);
    set("age", 17);
    set("long", @LONG
雪吟莊的年輕弟子﹐刀法初窺門徑﹐眉宇間帶著幾分名門子弟的傲氣。
LONG
    );
    set_skill("unarmed", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_skill("blade", 20);
    set("custom_faction", "fighter.snow");   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("C");
    setup();
    carry_object("/obj/area/obj/blade")->wield();
    carry_object("/obj/area/obj/cloth")->wear();
}
