// /d/snowkeep/npc/wang_hsian.c — 雪吟莊 NPC：王賢（Wang hsian）。
// 設定依使用者提供的雪吟莊設計表（2026-11-06）：人類、25 歲、武者、等級 20、強度 C。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("王賢", ({ "wang hsian", "wang", "hsian" }));
    set_race("human");
    set_class("fighter");
    set_level(20);
    set("gender", "male");
    set("age", 25);
    set("long",
        "王賢是雪亭鎮的王員外家雇用的護院武師﹐他的武藝跟此次來到雪吟山莊作客的\n"
        "武林大豪相比﹐實在是不足一哂﹐但是卻厚著臉皮靠著王員外的關係弄了一張英雄\n"
        "帖﹐大搖大擺地進出雪吟山莊﹐不過似乎沒有什麼人理睬他。\n");
    set_power("C");
    setup();
    carry_object("/obj/area/obj/leather_boot")->wear();        // 皮短靴
    carry_object("/custom/armor/armor/heart_plate")->wear();   // 護心鏡
    carry_object("/obj/area/obj/blade")->wield();              // 單刀
}
