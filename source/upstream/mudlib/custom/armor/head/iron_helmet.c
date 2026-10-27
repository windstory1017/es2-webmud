/* 鐵盔 — ES2 原始護具資料。振武軍營徵兵官魯熙年等 NPC 穿戴。 */
#include <ansi.h>
#include <armor.h>

inherit F_HEAD_EQ;

void create()
{
    set_name("鐵盔", ({ "iron helmet", "helmet" }));
    set_weight(2000);
    setup_head_eq();

    if( !clonep() ) {
        set("unit", "個");
        set("value", 1500);
        set("long",
            "一個沈重的鐵製頭盔 ... 光看就讓人脖子酸。\n");
        set("wear_as", "head_eq");
        set("apply_armor/head_eq", ([
            "armor": 3,
            "dex": -2,
        ]));
    }
    setup();
}
