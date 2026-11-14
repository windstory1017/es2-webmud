// /d/shuiyue/npc/old_tu.c — 水月村 NPC：村長 老土（村長家）。

#include <npc.h>

inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("老土", ({ "old tu", "tu", "chief" }));
    set("title", "村長");
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "male");
    set("age", 55);
    set("long", "老土是水月村的村長，花白的頭髮、滿臉的皺紋，精神卻十分矍鑠，手裡總握著一桿旱煙。村裡大小事務他都要過問，說起話來慢條斯理。\n");
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}
