/* 大刀 — ES2 原始武器資料。雪亭鎮鐵匠販售（/d/snow/npc/smith）。 */
#include <ansi.h>
#include <weapon.h>

inherit F_BLADE;

void create()
{
    set_name("大刀", ({ "large blade", "blade" }));
    set_weight(12000);
    init_damage(2, 18, 105, 0, "twohanded blade");

    if( !clonep() ) {
        set("wield_as", "twohanded blade");
        set("unit", "把");
        set("value", 4000);
        set("long",
            "又厚又重的大刀，令人望而生畏，如果雙手握持的話，威力更是強大。\n");
    }
    setup();
}
