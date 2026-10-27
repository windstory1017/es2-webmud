// /d/qiaoyin/npc/keeper.c — 喬陰縣 NPC：同仁藥鋪掌櫃（依使用者設計表，2026-10-16）。

#include <npc.h>
inherit F_VILLAGER;
inherit "/obj/alchemist/pill_vendor";   // 二階丹藥三倍價（使用者指示 #6，2026-10-23）

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

void create()
{
    seteuid(getuid());
    set_name("掌櫃", ({ "keeper" }));
    set("title", "同仁藥鋪");
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 50);
    set("long", @LONG
同仁藥鋪的掌櫃，一身半舊的長衫，袖口沾著些藥粉，對各種常用藥材與丹藥都十分熟悉。
LONG
    );
    set("merchandise", ([
        "/obj/alchemist/black_pill" : 30,                // 烏心丹
        "/obj/medication/boar_berry" : 20,                // 山豬果
        "/obj/medication/dragon_saliva" : 20,             // 龍涎草
        "/obj/alchemist/heat_furnace" : 20,                 // 青銅丹爐
        "/obj/alchemist/heat_reactor" : 50,                 // 青銅丹鼎
        "/obj/alchemist/heat_firewood" : 3000,                     // 松材
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
    carry_object("/obj/cloth")->wear();
}
