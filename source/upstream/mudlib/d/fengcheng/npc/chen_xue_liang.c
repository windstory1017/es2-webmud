// /d/fengcheng/npc/chen_xue_liang.c — 風城 NPC：劍甲門傳人 陳學亮（D16 交叉路口）。目前只放 NPC。

#include <npc.h>

inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("陳學亮", ({ "chen xue-liang", "chen", "xue-liang" }));
    set("title", "劍甲門傳人");
    set_race("human");
    set_class("alchemist");
    set_level(60);
    set("gender", "male");
    set("age", 35);
    set("long", "劍甲門的傳人，身形精瘦，雙手布滿了打鐵留下的老繭。劍甲門無門戶之見，但凡對鑄兵鍛甲有天份的人，他都肯傾囊相授。\n");
    set_skill("unarmed", 60);
    set_skill("dodge", 60);
    set_skill("parry", 60);
    set_power("A");
    setup();
}
