// /d/baihua/npc/san.c — 百花村 NPC：阿三哥（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("阿三哥", ({ "san" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 18);
    set("long", @LONG
一個身材壯碩的青年，皮膚曬得黝黑，兩條臂膀結實有力，村裡搬重物、修屋頂的粗活多半少不了他。他為人憨厚，說起話來有些靦腆，一見到村裡的小姑娘，臉就紅到了耳根。
LONG
    );
    set_power("C");
    setup();
}
