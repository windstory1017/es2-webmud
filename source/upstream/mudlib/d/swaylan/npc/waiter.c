// /d/swaylan/npc/waiter.c — 水嵐縣 NPC：五陵客棧店小二（照五堂鎮店小二改寫）。

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
    set_name("店小二", ({ "waiter" }));
    set("title", "五陵客棧");
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 35);
    set("long", @LONG
五陵客棧的店小二，肩上搭著一條抹布，手腳俐落，見了客人便堆起笑臉迎上來。你可以用 list 看可以點什麼，用 buy 跟店小二點菜。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value。
    set("merchandise", ([
        "/d/swaylan/obj/fishing_stick" : 10,
        "/obj/food/pork" : 50,
        "/d/swaylan/obj/chrysanthemum_skin" : 30,
        "/obj/food/manto" : 50,
        "/d/swaylan/obj/bait" : 300,
        "/d/swaylan/obj/noodle" : 20,
        "/d/swaylan/obj/white_wine" : 30,
    ]));
    set_power("C");
    setup();
}
