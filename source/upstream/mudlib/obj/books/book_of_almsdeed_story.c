// /obj/books/book_of_almsdeed_story.c — 【法華經】。依使用者設計表（2026-10-16）。

#include <ansi.h>

inherit ITEM;
inherit F_STUDY;

void create()
{
    set_name("【法華經】", ({ "book of almsdeed story", "almsdeed", "story", "book", "【法華經】" }));
    set_weight(1000);
    set("unit", "本");
    set("value", 500);   // 五兩銀子（照寶蓮寺的售價，使用者指示，2026-10-30）
    set("long", "一本【法華經】，以許多譬喻與布施的故事，闡明一切眾生皆可成佛的道理，文辭優美而意境深遠。\n");
    set("required/skill", (["literate": 120, "archaic attainment": 80, "absorption": 130]));
    set("content", (["literate": 160, "archaic attainment": 120, "absorption": 180]));
    setup();
}
