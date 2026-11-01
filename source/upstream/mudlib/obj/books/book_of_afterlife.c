// /obj/books/book_of_afterlife.c — 【往生經】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【往生經】", ({ "book of afterlife", "afterlife", "book", "【往生經】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 500);   // 五兩銀子（照寶蓮寺的售價，使用者指示，2026-10-30）
    set("long", "一本【往生經】，講述眾生臨終之際如何一心念佛、往生淨土，字裡行間充滿慈悲與安寧。\n");
    set("required/skill", (["literate": 60, "absorption": 75]));
    set("content", (["literate": 120, "absorption": 120]));
    setup();
}
