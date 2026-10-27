// /d/qiaoyin/npc/old_ho.c — 喬陰縣 NPC：老何（依使用者設計表，2026-10-16）。

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
    set_name("老何", ({ "oldman", "old ho", "ho" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 55);
    set("long", @LONG
在喬陰縣居住多年的雜貨舖老闆，總是能滿足大家的需求，偶爾還會把江述仁拿來賒帳的丹藥，轉賣給客人賺點差價。
LONG
    );
    set("merchandise", ([
        "/obj/alchemist/black_pill" : 30,                     // 烏心丹
        "/obj/alchemist/refresh_pill" : 20,    // 金鎖固精丸
        "/custom/item/scroll/blank" : 500,                // 空白符紙
        "/obj/alchemist/heat_firewood" : 2000,                     // 松材
        "/d/lee/obj/salt" : 50,                           // 鹽
        "/d/lee/obj/sugar" : 30,                          // 糖
    ]));
    set_power("C");
    setup();
    carry_object("/obj/cloth")->wear();
}
