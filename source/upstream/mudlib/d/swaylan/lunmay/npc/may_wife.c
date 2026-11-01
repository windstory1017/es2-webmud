// /d/swaylan/lunmay/npc/may_wife.c — 冷梅莊 NPC：梅夫人（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("梅夫人", ({ "may" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "female");
    set("age", 31);
    set("long", @LONG
梅影風的夫人，倚在床頭，面色蒼白如紙，眉目間依稀可見當年的秀麗。她說話時聲音輕細，不時以手帕掩口輕咳。病了這些時日，莊裡上下請遍了大夫，卻始終查不出病因。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
    carry_object("/custom/armor/cloth/green_cloth")->wear();
    carry_object("/custom/armor/finger/pearl_ring")->wear();
}
