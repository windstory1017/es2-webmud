// /d/kangping/npc/innkeeper.c — 康平村 NPC：客店主人（小客店）（依使用者設計表，2026-10-20）。

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
    set_name("客店主人", ({ "innkeeper" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 45);
    set("long", @LONG
小客店的主人，一個面容和善的中年漢子。店裡就他一個人，既是掌櫃也是店小二，一會兒在櫃臺後算帳，一會兒又端著飯菜在桌間穿梭，忙得滿頭大汗，卻始終笑臉迎人。
LONG
    );
    set("merchandise", ([
        "/d/snow/npc/obj/roast_chicken" : 30,   // 紅燒雞
        "/obj/food/dumpling" : 20,               // 牛肉包子
        "/obj/food/pork" : 10,                   // 白切肉
    ]));
    set_power("C");
    setup();
}
