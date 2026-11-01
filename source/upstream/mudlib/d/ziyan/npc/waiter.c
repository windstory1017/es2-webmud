// /d/ziyan/npc/waiter.c — 紫煙鎮 NPC：紫煙小棧店小二（依水嵐縣設計表）。

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
    set_name("店小二", ({ "waiter" }));
    set("title", "紫煙小棧");
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 30);
    set("long", @LONG
紫煙小棧的店小二，肩上搭著一條油膩膩的抹布，見了客人便殷勤地招呼，嘴裡報著店裡的拿手菜。你可以用 list 看可以點什麼，用 buy 跟店小二點菜。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        "/d/ziyan/obj/white_wine" : 30,   // 白干酒：四兩銀子
        "/obj/food/manto" : 50,           // 饅頭：十文錢
        "/d/ziyan/obj/vase" : 30,         // 果汁瓶：四兩銀子
        "/d/ziyan/obj/pig_heart" : 30,    // 豬心：四兩銀子
    ]));
    set_power("C");
    setup();
}
