// /d/swaylan/npc/man.c — 水嵐縣 NPC：路人（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("路人", ({ "man" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 30);
    set("long", @LONG
一個在街上閒逛的路人，東看看、西瞧瞧，似乎沒有甚麼要緊的事。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    set_power("C");
    setup();
}
