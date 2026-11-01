// /obj/books/book_of_mercy.c — 【大悲咒】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【大悲咒】", ({ "book of mercy", "mercy", "book", "【大悲咒】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 500);   // 五兩銀子（照寶蓮寺的售價，使用者指示，2026-10-30）
    set("long", "一本【大悲咒】，收錄觀世音菩薩廣大圓滿無礙大悲心陀羅尼，每一句咒文旁都註有梵音與釋義。\n");
    set("required/skill", (["literate": 70, "absorption": 50]));
    set("content", (["literate": 100, "absorption": 90]));
    setup();
}
