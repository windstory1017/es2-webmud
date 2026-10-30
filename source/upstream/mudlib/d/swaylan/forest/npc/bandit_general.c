// /d/swaylan/forest/npc/bandit_general.c — 密林 NPC：賊帥（依水嵐縣設計表第五批，docs/AREA_SWAYLAN.md）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("賊帥", ({ "bandit general", "general" }));
    set_race("yaksa");
    set_class("fighter");
    set_level(50);
    set("age", 22);
    set("long", @LONG
綠林大盜的總領賊帥，面如冠玉、劍眉星目，看上去不過二十出頭，右眼卻罩著一只黑皮眼罩，眼罩裡彷彿藏著什麼暗器，教人不敢與他對視。沒有人知道他的來歷，只知道他使得一手不俗的封山劍法。
LONG
    );
    set_skill("sword", 100);
    set_skill("fonxansword", 100);   // 封山劍法
    map_skill("sword", "fonxansword");
    map_skill("parry", "fonxansword");
    set_skill("dodge", 100);
    set_skill("parry", 100);
    set_skill("unarmed", 100);
    set("bounty", ([ "military service": 1000 ]));
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/needle_sword")->wield();      // 雙鬢鋏
    carry_object("/custom/armor/armor/dragon_jade_robe")->wear();    // 龍鱗璧玉袍
    carry_object("/d/swaylan/forest/obj/black_jade_pendant")->wear(); // 玄玉墜
    carry_money("silver", 5 + random(26));
}
