// /d/baihua/npc/wang_yean.c — 百花村 NPC：「花中菩提」王燕（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;
inherit F_VENDOR;

// 販賣肥遺骨粉與松柴（使用者定案，2026-10-21）。
void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

void create()
{
    seteuid(getuid());
    set_name("王燕", ({ "wang yean", "wang", "yean" }));
    set_race("human");
    set_class("alchemist");
    set_level(20);
    set("gender", "female");
    set("age", 20);
    set("long", @LONG
一個滿身藥味, 神情專注的女子正在清理一些木材, 似乎沒有注意到你正在看她. 由她的裝束來看, 不像學武之人, 反倒像是到處尋藥之神醫. 她師兄陳度好管閒事, 反倒在藥材的研究上, 比不上默默鑽研的王燕. 尤其專注的神貌, 似乎正在調理重要的配方.
LONG
    );
    set("nickname", "花中菩提");
    // 龍圖丹派（陳度的師妹）。只做門派記號，不顯示稱號（使用者指定）。
    set("custom_faction", "alchemist.longtu");
    set("merchandise", ([
        "/obj/food/snake_powder" : 100,              // 肥遺骨粉
        "/obj/alchemist/heat_firewood" : 5000,       // 松柴
    ]));
    set_power("B");
    setup();
    carry_object("/custom/weapon/dagger/shennong_dagger")->wield();
    carry_object("/custom/armor/cloth/green_cloth")->wear();
    carry_object("/custom/armor/head/goat_hat")->wear();
}
