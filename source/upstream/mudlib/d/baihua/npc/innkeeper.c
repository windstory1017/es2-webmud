// /d/baihua/npc/innkeeper.c — 百花村 NPC：掌櫃（紅燈客棧）（依使用者設計表，2026-10-20）。

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
    set_name("掌櫃", ({ "innkeeper" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 40);
    set("long", @LONG
紅燈客棧的掌櫃，一個瘦小的中年人，整天坐在櫃臺後面劈哩啪啦地打著算盤，嘴裡還念念有詞，客人上門也不太抬頭，算盤珠子倒是一刻也沒停過。
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
