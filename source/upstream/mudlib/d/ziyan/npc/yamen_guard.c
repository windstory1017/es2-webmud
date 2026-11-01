// /d/ziyan/npc/yamen_guard.c — 紫煙鎮 NPC：衙役（依水嵐縣設計表；數值使用者同意，2026-10-30）。

#include <npc.h>
inherit F_SOLDIER;

void create()
{
    seteuid(getuid());
    set_name("衙役", ({ "guard" }));
    set_race("human");
    set_class("soldier");
    set_level(15);
    set("age", 30);
    set("long", @LONG
縣衙裡跑腿打雜的衙役，腰間掛著一條鐵鞭，一臉不耐煩的神氣，平日裡最大的本事便是狐假虎威。
LONG
    );
    set_skill("unarmed", 30);
    set_skill("dodge", 30);
    set_skill("parry", 30);
    set_skill("blunt", 30);
    set_power("B");
    setup();
    carry_object("/obj/area/obj/rod")->wield();
}
