// /d/swaylan/npc/pawn_guard.c — 水嵐縣 NPC：高陽當鋪守衛（依水嵐縣設計表）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("高陽當鋪守衛", ({ "guard" }));
    set_race("blackteeth");
    set_class("fighter");
    set_level(10);
    set("age", 40);
    set("long", @LONG
一個膚色黝黑、身材魁梧的漢子，雙臂粗壯，抱著胳膊立在當鋪門邊，誰多看他一眼，他便瞪回去。
LONG
    );
    set_skill("unarmed", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_power("C");
    setup();
}
