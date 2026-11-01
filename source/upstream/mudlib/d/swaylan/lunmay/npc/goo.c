// /d/swaylan/lunmay/npc/goo.c — 冷梅莊 NPC：古大叔（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("古大叔", ({ "goo" }));
    set("title", "冷梅莊老僕");
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "male");
    set("age", 70);
    set("long", @LONG
梅家的老長工，背已經有些駝了，手腳卻還俐落，整天在莊裡忙進忙出。他在冷梅莊待了大半輩子，莊裡大小事沒有他不知道的。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
    carry_object("/obj/area/obj/cloth")->wear();
}
