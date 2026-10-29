// /obj/books/book_of_herbal_meridians.c — 【藥石脈錄】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【藥石脈錄】", ({ "book of herbal meridians", "herbal", "meridians", "book", "【藥石脈錄】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 200);
    set("long", "一本【藥石脈錄】，將藥石之性與經脈之理相互參照，記載何種藥材能通何處經脈，非精通藥理者難以讀懂。\n");
    set("required/skill", (["literate": 125, "kee anatomy": 80, "alchemy-medication": 100, "cauterization": 1]));
    set("content", (["kee anatomy": 160]));
    setup();
}
