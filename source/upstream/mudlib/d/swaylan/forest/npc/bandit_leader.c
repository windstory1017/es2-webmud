// /d/swaylan/forest/npc/bandit_leader.c — 密林 NPC：小頭目（依水嵐縣設計表第五批，docs/AREA_SWAYLAN.md）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("小頭目", ({ "bandit leader", "leader" }));
    set_race("human");
    set_class("thief");
    set_level(15);
    set("age", 25);
    set("long", @LONG
盜賊裡管著一小隊人的頭目，身上穿著一副翠玉甲，在一群破衣爛衫的盜賊中顯得格外神氣，說話時總愛揚著下巴。
LONG
    );
    set_skill("sword", 30);
    set_skill("dodge", 30);
    set_skill("parry", 30);
    set_skill("unarmed", 30);
    set("bounty", ([ "military service": 100 ]));
    set_power("B");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/d/swaylan/forest/obj/jade_armor")->wear();
    carry_money("silver", 5 + random(26));
}
