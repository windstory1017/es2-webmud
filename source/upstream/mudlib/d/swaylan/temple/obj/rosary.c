// /d/swaylan/temple/obj/rosary.c — 念珠（寶蓮寺的住持與小尼姑配戴，依水嵐縣設計表）。
#include <armor.h>

inherit F_NECK_EQ;

void create()
{
    set_name("念珠", ({ "rosary" }));
    set_weight(200);
    setup_neck_eq();

    if( !clonep() ) {
        set("unit", "串");
        set("value", 500);
        set("long", "一串木頭念珠，珠子被長年捻動磨得圓潤光亮，隱隱帶著一股檀香。\n");
        set("wear_as", "neck_eq");
        set("apply_armor/neck_eq", ([
            "int": 1,
        ]));
    }
    setup();
}
