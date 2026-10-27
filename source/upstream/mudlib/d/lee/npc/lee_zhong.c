// /d/lee/npc/lee_zhong.c
// NEW（李家村復原版新增角色，非原始 ES2 資料，由專案成員指定人設）

#include <npc.h>
inherit F_VILLAGER;
inherit F_VENDOR;

void create()
{
    set_name("李忠", ({ "lee zhong", "li zhong", "zhong", "lee", "li" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("long", "李忠是賣雜貨小夥計，整天笑嘻嘻的，一臉老實樣。\n");
    // merchandise 的數字是庫存份數，不是價錢——價錢是各道具檔自己的 "value"。
    set("merchandise", ([
        "/d/lee/obj/waterskin" : 20,
        "/obj/area/obj/bag" : 10,
        "/d/lee/obj/salt" : 100,
        "/d/lee/obj/sugar" : 100,
        "/d/lee/obj/flour" : 100,
        "/custom/item/scroll/blank" : 1000,   // 空白符紙（2026-10-24）
    ]));
    set_power("C");
    setup();
}

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}
