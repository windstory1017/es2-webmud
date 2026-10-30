// /d/swaylan/npc/cutter.c — 水嵐縣 NPC：樵夫（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("樵夫", ({ "cutter" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 40);
    set("long", @LONG
一個皮膚黝黑的中年漢子，肩上扛著一柄磨得發亮的柴刀，腰間纏著草繩。他常年在城外林子裡砍柴，挑進城裡換幾個錢，一雙手掌滿是厚繭。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}
