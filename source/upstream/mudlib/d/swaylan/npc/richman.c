// /d/swaylan/npc/richman.c — 水嵐縣 NPC：錢記商號 錢富貴（依水嵐縣設計表）。

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
    set_name("錢富貴", ({ "richman", "chien" }));
    set("title", "錢記商號");
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "female");
    set("age", 55);
    set("long", @LONG
一位衣著華貴的婦人，手上戴著幾只金鐲子，算盤打得飛快。她是水嵐縣數一數二的富戶，生意上分毫不讓，修橋鋪路、施粥濟貧卻從不落人後，鄉里提起她無不敬重。你可以用 list 看她賣些什麼，用 buy 購買。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        "/obj/alchemist/heat_furnace" : 30,      // 青銅丹爐
        "/obj/alchemist/heat_reactor" : 50,      // 青銅丹鼎
        "/obj/alchemist/heat_firewood" : 10000,  // 松柴
        "/obj/medication/boar_berry" : 30,       // 山豬果
        "/obj/alchemist/black_pill" : 50,        // 烏心丹
        "/obj/medication/wild_ginseng" : 20,     // 野山蔘
    ]));
    set_power("C");
    setup();
}
