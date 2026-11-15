// /d/shuiyue/npc/innkeeper.c — 水月村 NPC：老闆娘（福安客棧），賣饅頭與大饅頭。

#include <npc.h>

inherit F_VILLAGER;
inherit F_VENDOR;

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

void create()
{
    seteuid(getuid());
    set_name("老闆娘", ({ "woman", "innkeeper", "landlady" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "female");
    set("age", 55);
    set("long", "福安客棧的老闆娘，圓圓的臉上掛著熱誠的笑，腰間繫著油漬斑斑的圍裙，招呼客人手腳俐落，嘴裡不停地吆喝。你可以用 list 看她賣些什麼，用 buy 購買。\n");
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        "/obj/food/manto" : 50,
        "/d/shuiyue/obj/big_manto" : 50,
    ]));
    set_power("C");
    setup();
}
