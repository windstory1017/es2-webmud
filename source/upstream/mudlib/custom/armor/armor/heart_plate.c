/* 護心鏡 — 舊站裝備（王賢身上）。名稱取自設計表；數值由使用者提供（2026-11-06）：
 * 防禦 10、守勢等級 10。比照 custom/armor/armor/heart_armor.c。NEW */
#include <armor.h>

inherit F_ARMOR;

void create()
{
    set_name("護心鏡", ({ "heart plate", "plate" }));
    set_weight(6000);
    setup_armor();

    if( !clonep() ) {
        set("unit", "面");
        set("value", 2000);
        set("long",
            "一面打磨得發亮的圓形銅鏡，用皮帶繫在胸前，正好護住心口要害。\n");
        set("wear_as", "armor");
        set("apply_armor/armor", ([
            "armor": 10,
            "wittiness": 10,
        ]));
    }
    setup();
}
