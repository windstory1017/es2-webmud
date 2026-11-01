// /d/swaylan/forest/obj/bi_jade_armor.c — 璧玉甲（密林大頭目穿，使用者指示，2026-10-30）：根骨 +1、慧根 +1、防禦力 15。
#include <armor.h>

inherit F_ARMOR;

void create()
{
    set_name("璧玉甲", ({ "bi-jade armor", "armor" }));
    set_weight(5000);
    setup_armor();

    if( !clonep() ) {
        set("unit", "副");
        set("value", 8000);
        set("long",
            "一副用白璧玉片綴成的甲衣，每片玉都雕成圓璧的形狀，中間穿孔，以銀絲\n"
            "串連得嚴嚴實實，色澤溫潤如脂，比尋常的玉甲要講究得多。\n");
        set("wear_as", "armor");
        set("apply_armor/armor", ([
            "armor": 15,
            "con": 1,
            "wis": 1,
        ]));
    }
    setup();
}
