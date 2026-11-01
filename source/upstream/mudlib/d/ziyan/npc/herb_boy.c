// /d/ziyan/npc/herb_boy.c — 紫煙鎮 NPC：小藥童（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("小藥童", ({ "herb boy", "boy" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 12);
    set("long", @LONG
藥鋪裡打雜的小藥童，手上沾滿了藥渣，正蹲在竹篩邊翻曬藥材，被藥味燻得直打噴嚏。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    set_power("C");
    setup();
}
