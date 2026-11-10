/* 虹緞帶 — 百花村小巷小姑娘（/d/baihua/npc/girl.c）繫在腰間的緞帶（使用者指定，2026-11-06）。 */
#include <ansi.h>
#include <armor.h>

inherit F_WAIST_EQ;

void create()
{
    set_name("虹緞帶", ({ "rainbow ribbon", "ribbon" }));
    set_weight(50);
    setup_waist_eq();

    if( !clonep() ) {
        set("unit", "條");
        set("value", 3000);
        set("long",
            "一條質地柔軟的緞帶，七色絲線由紅至紫漸次相染，色澤流轉如天邊的虹彩。"
            "鬆鬆繫在腰間，隨著步子輕輕飄動，彷彿有一道小小的彩虹繞在身側，教人看了心情也跟著明亮起來。\n");
        set("wear_as", "waist_eq");
        set("apply_armor/waist_eq", ([
            "spi": 1,
            "armor": 3,
        ]));
    }
    setup();
}
