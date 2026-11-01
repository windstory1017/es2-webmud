// /d/ziyan/npc/herbalist.c — 紫煙鎮 NPC：藥鋪掌櫃（依水嵐縣設計表）。

#include <npc.h>
inherit F_VILLAGER;
inherit "/obj/alchemist/pill_vendor";   // 丹藥依階級定價，同李家村藥鋪

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

void create()
{
    seteuid(getuid());
    set_name("藥鋪掌櫃", ({ "herbalist", "keeper" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 60);
    set("long", @LONG
一個瘦小的老頭，鼻樑上架著一副老花眼鏡，抓藥時一手撥著戥子，一手翻著藥櫃，嘴裡念念有詞，從來不會抓錯一味。你可以用 list 看他賣些什麼，用 buy 購買。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        STOCK_MEDICATION("dragon_saliva") : 50,      // 龍涎草
        "/obj/alchemist/black_pill" : 30,            // 烏心丹
        "/obj/alchemist/minor_heal_pill" : 20,       // 小還丹（設計表寫作小環丹）
        "/obj/alchemist/mana_pill" : 30,             // 醒神丹
    ]));
    set_power("C");
    setup();
}
