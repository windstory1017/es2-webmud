// /d/wutang/npc/tao.c — 五堂鎮 NPC：小桃（依五堂鎮設計表）。

#include <npc.h>
inherit F_VILLAGER;
inherit F_VENDOR;

void create()
{
    seteuid(getuid());
    set_name("小桃", ({ "tao", "xiao tao" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "female");
    set("age", 10);
    set("long", @LONG
天真爛漫的小女孩，正盯著布莊李美麗的花布。
LONG
    );
    // 布莊販售的衣物（2026-10-24）；數字是庫存份數，價錢是各衣物檔自己的 "value"。
    set("merchandise", ([
        "/custom/armor/cloth/black_mandarin_jacket" : 5,
        "/custom/armor/cloth/embroidery_dress" : 5,
        "/custom/armor/cloth/pink_dress" : 5,
        "/custom/armor/cloth/white_cloth" : 2,
        "/d/snow/npc/obj/white_dress" : 2,
    ]));
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}
