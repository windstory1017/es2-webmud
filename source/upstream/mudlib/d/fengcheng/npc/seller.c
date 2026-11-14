// /d/fengcheng/npc/seller.c — 風城 NPC：小販（D18 南街）。賣供品、空白符紙、龍麟袋。

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
    set_name("小販", ({ "seller" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "male");
    set("age", 18);
    set("long", "一個年輕的小販，守著一副擔子在街邊做買賣。你可以用 list 看他賣些什麼，用 buy 購買。\n");
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        "/d/fengcheng/obj/offering" : 10,
        "/custom/item/scroll/blank" : 2000,
        "/custom/item/dragon_skin_bag" : 5,
    ]));
    set_power("C");
    setup();
}
