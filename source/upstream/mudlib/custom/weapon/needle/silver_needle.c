/* 銀針 — ES2 原始武器資料。所屬 NPC 另行設定。 */
#include <ansi.h>
#include <weapon.h>

inherit F_NEEDLE;

void create()
{
    set_name("銀針", ({ "silver needle", "needle" }));
    set_weight(500);
    init_damage(2, 5, 80, 2, "needle");
    init_damage(2, 5, 80, 2, "secondhand needle");

    if( !clonep() ) {
        set("wield_as", ({ "needle", "secondhand needle" }));
        set("unit", "把");
        set("value", 200);
        set("long",
            "一支銀色的短灸針。\n");
    }
    setup();
}
