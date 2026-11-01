// /obj/books/book_of_five_meridians.c — 【五脈內經】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【五脈內經】", ({ "book of five meridians", "five", "meridians", "book", "【五脈內經】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 200);
    set("long", "一本【五脈內經】，詳細記載五臟所屬經脈的走向與相生相剋之理，書中附有許多精細的經絡圖。\n");
    set("required/skill", (["literate": 60, "kee anatomy": 80, "cauterization": 1]));
    set("content", (["kee anatomy": 160]));
    setup();
}
