// /d/swaylan/npc/smith.c — 水嵐縣 NPC：鐵匠（鐵匠鋪賣東西的人，使用者同意加上，2026-10-30）。

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
    set_name("鐵匠", ({ "smith", "blacksmith" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 45);
    set("long", @LONG
一個打著赤膊的壯漢，臂膀被爐火烤得黝黑發亮，手中鐵鎚起落之間，火星四濺。你可以用 list 看他賣些什麼，用 buy 購買。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value；一律普通品。
    set("merchandise", ([
        "/obj/area/obj/shortsword" : 5,
        "/obj/area/obj/dagger" : 5,
        "/custom/weapon/needle/silver_needle" : 5,
        "/d/snow/npc/obj/hoe" : 5,
    ]));
    set_power("C");
    setup();
}
