// /d/graveyard/npc/barbarian.c — 亂葬崗 NPC：野人（barbarian），跟著野人酋長住在密林裏（依亂葬崗設計表）。
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("野人", ({ "barbarian" }));
    set_race("human");
    set_class("fighter");
    set_level(12);
    set("gender", "male");
    set("age", 18);
    set("long", "一個赤身露體的野人﹐看起來相當孔武有力。\n");
    set_attr("str", 20);
    set_power("B");
    setup();
}
