#include <npc.h>
inherit F_VILLAGER;
inherit "/obj/alchemist/pill_vendor";   // 二階丹藥三倍價（使用者指示 #6，2026-10-23）

void create()
{
    set_name("李岩", ({ "lee yen", "li yan", "herbalist", "lee", "yen", "li", "yan" }));
    set_race("human");
    set_class("commoner");
    set_level(4);
    set("age", 43);
    set("long", "李家村藥鋪的掌櫃，熟悉常用藥材與丹藥。你可以用 list 查看目前已恢復且功能可核對的商品。\n");
    /*
     * 原資料可確認的藥鋪品項比目前核心可直接支援的更多。
     * 先使用現存 canonical object，避免把未知效果硬寫成既定功能。
     */
    set("merchandise", ([
        "/obj/alchemist/black_pill" : 50,
        "/obj/medication/boar_berry" : 40,
        "/obj/alchemist/heat_firewood" : 30,
        "/obj/alchemist/heat_reactor" : 10,
        "/obj/alchemist/heat_furnace" : 5,
        "/d/lee/obj/medicine_bottle" : 20,
        "/d/lee/obj/heaven_dew" : 10,
        "/obj/alchemist/minor_heal_pill" : 10,           // 小還丹
        "/obj/alchemist/mana_pill" : 10,                 // 醒神丹
        "/obj/alchemist/aquapill" : 50,                  // 金液丹
        "/obj/alchemist/refresh_pill" : 10,              // 金鎖固精丸
        "/obj/alchemist/lavage_pill" : 5,              // 清腹丹
        "/obj/alchemist/relax_pill" : 5,               // 養心丹
        // 以上丹藥數量依使用者指示 #5（2026-10-23）
    ]));
    set_power("C");
    setup();
}

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}
