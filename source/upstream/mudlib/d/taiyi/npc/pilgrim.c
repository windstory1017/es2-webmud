// /d/taiyi/npc/pilgrim.c — 太乙觀：進香客（依太乙觀設計表，2026-11-09）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("進香客", ({ "pilgrim" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 32);
    set("long", @LONG
一位遠道而來進香的香客，衣上還沾著趕路的塵土，神色卻很虔誠。
LONG
    );
    set_power("C");
    setup();
    carry_money("silver", 40 + random(11));   // 身上帶 40～50 銀，無裝備
}
