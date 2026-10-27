// /d/baihua/npc/girl.c — 百花村 NPC：小姑娘（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("小姑娘", ({ "girl" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "female");
    set("age", 14);
    set("long", @LONG
一個模樣清秀的小姑娘，烏黑的頭髮梳成兩條辮子，笑起來臉頰上有兩個淺淺的酒窩。她心地善良，常常煮了熱騰騰的羹湯送給村裡的老人家，說起阿三哥的時候，眼神總是特別溫柔。
LONG
    );
    set_power("C");
    setup();
}
