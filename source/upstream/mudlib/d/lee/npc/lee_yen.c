#include <npc.h>
inherit F_VILLAGER;
inherit F_VENDOR;

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
        "/obj/medication/black_pill" : 50,
        "/obj/medication/boar_berry" : 40,
        "/obj/alchemist/heat_firewood" : 30,
        "/obj/alchemist/heat_reactor" : 10,
        "/obj/alchemist/heat_furnace" : 5,
        "/d/lee/obj/medicine_bottle" : 20,
        "/d/lee/obj/heaven_dew" : 10,
    ]));
    set_power("C");
    setup();
}

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}
