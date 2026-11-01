// /d/swaylan/npc/zhang_ling.c — 水嵐縣 NPC：張齡（依水嵐縣設計表）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("張齡", ({ "zhang ling", "zhang", "ling" }));
    set_race("human");
    set_class("taoist");
    set_level(30);
    set("gender", "male");
    set("age", 65);
    set("long", @LONG
一位鬚髮斑白的老者，腰間佩著一柄古劍，身上卻穿著道袍。據說他年輕時是個劍客，後來沉迷法術，竟將劍術與道法融為一爐，自創了一套幻劍術。
LONG
    );
    set_skill("unarmed", 60);
    set_skill("dodge", 60);
    set_skill("parry", 60);
    set_skill("sword", 60);
    set_power("B");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/obj/area/obj/robe")->wear();
}
