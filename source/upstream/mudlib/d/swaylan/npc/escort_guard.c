// /d/swaylan/npc/escort_guard.c — 水嵐縣 NPC：黃家鏢局鏢師（照五堂鎮護院武師改寫）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("黃家鏢局鏢師", ({ "guard" }));
    set_race("blackteeth");
    set_class("fighter");
    set_level(5);
    set("age", 28);
    set("long", @LONG
黃家鏢局的鏢師，一身勁裝，腰懸佩刀，目光沉穩，守在內宅門前寸步不離。
LONG
    );
    set_power("B");
    setup();
    carry_object("/custom/armor/cloth/cowhide_vest")->wear();
    carry_object("/obj/area/obj/blade")->wield();
}
