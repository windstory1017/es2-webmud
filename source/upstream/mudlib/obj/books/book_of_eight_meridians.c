// /obj/books/book_of_eight_meridians.c — 【八脈注】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【八脈注】", ({ "book of eight meridians", "eight", "meridians", "book", "【八脈注】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 200);
    set("long", "一本【八脈注】，專論奇經八脈的循行與交會，註解艱深，是醫家與武人都夢寐以求的秘本。\n");
    set("required/skill", (["literate": 120, "kee anatomy": 160, "cauterization": 1]));
    set("content", (["kee anatomy": 200]));
    setup();
}
