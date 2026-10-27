/* 連身鎖子甲 — ES2 原始護具資料。振武軍營徵兵官魯熙年等 NPC 穿戴。 */
#include <ansi.h>
#include <armor.h>

inherit F_ARMOR;

void create()
{
    set_name("連身鎖子甲", ({ "ring armor", "armor" }));
    set_weight(8000);
    setup_armor();

    if( !clonep() ) {
        set("unit", "件");
        set("value", 5000);
        set("long",
            "一件連身鎖子甲﹐由鐵鍊接合許多鐵片﹐構成保護全身的一件戰甲﹐因\n"
            "為嵌上鐵片﹐所以比一般的鎖子鎧有更好的防禦效果﹐但是重量也重得\n"
            "多。\n");
        set("wear_as", "armor");
        set("apply_armor/armor", ([
            "dodge": -5,
            "armor": 9,
        ]));
    }
    setup();
}
