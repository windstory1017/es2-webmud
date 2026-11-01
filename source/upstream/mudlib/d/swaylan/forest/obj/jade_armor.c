// /d/swaylan/forest/obj/jade_armor.c — 翠玉甲（密林的小頭目、大頭目穿，依水嵐縣設計表）：悟性 +1、防禦力 10。
#include <armor.h>

inherit F_ARMOR;

void create()
{
    set_name("翠玉甲", ({ "jade armor", "armor" }));
    set_weight(5000);
    setup_armor();

    if( !clonep() ) {
        set("unit", "副");
        set("value", 5000);
        set("long",
            "一副用碧綠玉片串成的甲衣，玉片打磨得溫潤光滑，穿在身上叮噹作響，\n"
            "不知是從哪家富戶搶來的。\n");
        set("wear_as", "armor");
        set("apply_armor/armor", ([
            "armor": 10,
            "int": 1,
        ]));
    }
    setup();
}
