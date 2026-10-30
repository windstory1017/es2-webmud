// /d/swaylan/forest/npc/cooker.c — 密林 NPC：老王（賊窩的廚子，依水嵐縣設計表；數值使用者同意，2026-10-30）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("老王", ({ "cooker", "wang" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 45);
    set("long", @LONG
賊窩裡掌勺的廚子，圍著一條油膩膩的圍裙，揮著菜刀剁肉剁得砰砰作響，對來人愛理不理。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}
