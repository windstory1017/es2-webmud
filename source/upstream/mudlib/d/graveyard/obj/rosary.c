// /d/graveyard/obj/rosary.c — 念珠（亂葬崗的老和尚、小和尚配戴，依亂葬崗設計表）。
#include <armor.h>

inherit F_NECK_EQ;

void create()
{
    set_name("念珠", ({ "rosary" }));
    set_weight(20);
    setup_neck_eq();

    if( !clonep() ) {
        set("unit", "串");
        set("value", 1200);
        set("long", "一串木頭念珠，珠子被捻得圓潤光亮，隱隱透著一股檀香。\n");
        set("wear_as", "neck_eq");
        set("apply_armor/neck_eq", ([
            "int": 1,
        ]));
    }
    setup();
}
