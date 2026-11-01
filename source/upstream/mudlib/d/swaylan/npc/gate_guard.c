// /d/swaylan/npc/gate_guard.c — 水嵐縣 NPC：城門守衛（依水嵐縣設計表）。不攔人。

#include <npc.h>
inherit F_SOLDIER;

// 出生時隨機拿其中一種兵器（設計表）。
string *weapons = ({
    "/obj/area/obj/pike",
    "/obj/area/obj/weirblade",
    "/obj/area/obj/broadsword",
    "/custom/weapon/axe/huge_axe",
});

void create()
{
    seteuid(getuid());
    set_name("城門守衛", ({ "guard" }));
    set_race("human");
    set_class("soldier");
    set_level(20);
    set("age", 25);
    set("long", @LONG
一名身披鐵甲的官兵，手執兵器守在城門邊，兩眼不住地打量著進出的行人，神情一絲不苟。
LONG
    );
    set_skill("unarmed", 40);
    set_skill("dodge", 40);
    set_skill("parry", 40);
    set_skill("twohanded pike", 40);
    set_skill("blade", 40);
    set_skill("sword", 40);
    set_skill("twohanded axe", 40);
    set_power("A");
    setup();
    carry_object(weapons[random(sizeof(weapons))])->wield();
    carry_object("/custom/armor/armor/ring_mail")->wear();
    carry_object("/d/snow/npc/obj/clothboot")->wear();
}
