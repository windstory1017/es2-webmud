// /obj/books/book_of_anatomy.c — 【脈穴內經】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【脈穴內經】", ({ "book of anatomy", "anatomy", "book", "【脈穴內經】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 200);
    set("long", "一本【脈穴內經】，書的封面上是人體的穴位圖。\n");
    set("required/skill", (["literate": 25, "kee anatomy": 15, "cauterization": 1]));
    set("content", (["kee anatomy": 100]));
    setup();
}
