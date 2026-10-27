/* 精鋼砍刀 — ES2 原始武器資料。振武軍營徵兵官魯熙年等 NPC 穿戴。 */
#include <ansi.h>
#include <weapon.h>

inherit F_BLADE;

void create()
{
    set_name("精鋼砍刀", ({ "steelblade", "blade" }));
    set_weight(14500);
    init_damage(3, 16, 105, 0, "twohanded blade");

    if( !clonep() ) {
        set("wield_as", "twohanded blade");
        set("unit", "把");
        set("value", 75000);
        set("long",
            "這是一把精鋼打造的砍刀﹐刀面極闊﹐戰場上廝殺相當有用﹐但是\n"
            "用於武林中鬥毆卻顯得有些笨重了。\n");
    }
    setup();
}
