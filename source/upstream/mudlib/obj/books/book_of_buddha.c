// /obj/books/book_of_buddha.c — 【佛名經】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【佛名經】", ({ "book of buddha", "buddha", "book", "【佛名經】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 500);   // 五兩銀子（照寶蓮寺的售價，使用者指示，2026-10-30）
    set("long", "一本【佛名經】，密密麻麻地列出過去、現在、未來諸佛的名號，據說虔心誦讀可以消除罪業。\n");
    set("required/skill", (["literate": 100, "archaic attainment": 60, "absorption": 100]));
    set("content", (["literate": 130, "archaic attainment": 90, "absorption": 150]));
    setup();
}
