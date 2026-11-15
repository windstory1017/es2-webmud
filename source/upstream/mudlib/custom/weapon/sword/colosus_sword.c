/* 巨劍 — ES2 原始武器資料。所屬 NPC 另行設定。 */
#include <ansi.h>
#include <weapon.h>

inherit F_SWORD;

void create()
{
    set_name("巨劍", ({ "colosus sword", "sword" }));
    set_weight(16900);
    init_damage(3, 18, 80, 6, "twohanded sword");

    if( !clonep() ) {
        set("wield_as", "twohanded sword");
        set("unit", "把");
        set("value", 35000);
        set("long",
            "一把約五尺長的巨劍﹐份量奇重，需要相當的膂力才能揮舞這種武器。\n");
    }
    setup();
    // 武器附加屬性：每把新產生的巨劍擲一次前綴／後綴，出詞綴的機會是一般武器的 3 倍（adm/daemons/enhanced.c）。
    if( clonep() ) ENHANCE_D->roll_affix(this_object(), 3);
}
