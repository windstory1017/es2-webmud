// /d/graveyard/obj/chrysoberyl_ring.c — 貓眼戒指（亂葬崗祠堂的邋遢道士身上，依亂葬崗設計表）。
#include <armor.h>

inherit F_FINGER_EQ;

void create()
{
    set_name("貓眼戒指", ({ "chrysoberyl ring", "ring" }));
    set_weight(100);
    setup_finger_eq();

    if( !clonep() ) {
        set("unit", "枚");
        set("value", 3000);
        set("long", "一枚鑲著貓眼石的銅戒﹐你發現指環內面刻著「雍順」兩個小字。\n");
        set("wear_as", "finger_eq");
        set("apply_armor/finger_eq", ([
            "cps": 1,
        ]));
    }
    setup();
}
