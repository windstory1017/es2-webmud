/* 精鐵環扣 — ES2 原始護具資料。振武軍營徵兵官魯熙年等 NPC 穿戴。 */
#include <ansi.h>
#include <armor.h>

inherit F_WAIST_EQ;

void create()
{
    set_name("精鐵環扣", ({ "iron girth", "girth" }));
    set_weight(800);
    setup_waist_eq();

    if( !clonep() ) {
        set("unit", "個");
        set("value", 600);
        set("long",
            "這是一般上陣武官用來繫扣戰甲用的環扣﹐可以避免鎧甲脫落。\n");
        set("wear_as", "waist_eq");
        set("apply_armor/waist_eq", ([
            "armor": 2,
        ]));
    }
    setup();
}
