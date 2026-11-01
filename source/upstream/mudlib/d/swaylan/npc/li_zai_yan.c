// /d/swaylan/npc/li_zai_yan.c — 水嵐縣 NPC：李再延（依水嵐縣設計表）。

#include <npc.h>
inherit F_SCHOLAR;

void create()
{
    seteuid(getuid());
    set_name("李再延", ({ "li zai yan", "li", "zai yan" }));
    set_race("human");
    set_class("scholar");
    set_level(10);
    set("gender", "male");
    set("age", 38);
    set("long", @LONG
一個三十多歲的書生，神態悠閒，喜歡與人攀談，三言兩語便能把話題扯到天南地北，似乎甚麼事都想問個究竟。
LONG
    );
    set_skill("unarmed", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_power("C");
    setup();
}
