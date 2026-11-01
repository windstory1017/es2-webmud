// /d/swaylan/buxuan/npc/apprentice.c — 步玄派 NPC：步玄派弟子（依水嵐縣設計表，寫法參考冷梅莊弟子）。

#include <npc.h>
inherit F_SCHOLAR;

void create()
{
    seteuid(getuid());
    set_name("步玄派弟子", ({ "apprentice" }));
    set_race("human");
    set_class("scholar");
    set_level(10);
    set("age", 20);
    set("long", @LONG
步玄派的弟子，身穿樸素的布衣，腰間佩著長短兩劍，眉目間透著幾分書卷氣，平日耕讀習武，舉止從容有禮。
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
    carry_object("/obj/area/obj/cloth")->wear();
}
