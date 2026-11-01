// /d/swaylan/forest/obj/black_jade_pendant.c — 玄玉墜（賊帥配戴，依水嵐縣設計表）：
// 項鍊；守勢等級 −20、定力 −2、膽識 +2；名稱暗紅色。
#include <ansi.h>
#include <armor.h>

inherit F_NECK_EQ;

void create()
{
    set_name(RED "玄玉墜" NOR, ({ "black jade pendant", "pendant" }));
    set_weight(200);
    setup_neck_eq();

    if( !clonep() ) {
        set("unit", "塊");
        set("value", 8000);
        set("long",
            "一塊暗紅色的玉墜，色澤深沉如凝固的血，握在手裡隱隱發燙，戴上之後，\n"
            "心頭彷彿多了一股不管不顧的狠勁。\n");
        set("wear_as", "neck_eq");
        set("apply_armor/neck_eq", ([
            "wittiness": -20,
            "cps": -2,
            "cor": 2,
        ]));
    }
    setup();
}
