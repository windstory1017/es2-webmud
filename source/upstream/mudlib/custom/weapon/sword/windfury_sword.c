/* 逐風刃 — 駱易的佩劍，與落雲刃本是一對（使用者定案，2026-11-01）。
 *
 * - 傷害、重量、價值同落雲刃（cloudslay_sword.c）；敘述以落雲刃為基礎撰寫；名稱亮青色。
 * - 左手：攻勢等級 50、火焰／冰寒／風擊／雷電傷害防禦力各 50。
 *   右手：攻勢等級 25、火焰／冰寒／風擊／雷電傷害防禦力各 25。
 * - 也是風泉之劍被玩家取走後，駱易重生時拿的替代品（風泉之劍的 replica_ob）。
 */
#include <ansi.h>
#include <weapon.h>

inherit F_SWORD;

void create()
{
    set_name(HIC "逐風刃" NOR, ({ "windfury sword", "windfury", "sword" }));
    set_weight(8700);
    init_damage(3, 18, 100, 6, "sword");
    init_damage(3, 18, 100, 6, "secondhand sword");

    if( !clonep() ) {
        set("wield_as", ({ "sword", "secondhand sword" }));
        set("unit", "把");
        set("value", 15000);
        set("long",
            "此劍劍身不長，刃薄如蟬翼，日光下映出一抹淡淡的青光，與落雲刃本\n"
            "是一對。揮動起來毫無阻力，劍過之處帶起一縷輕風，彷彿追著風勢而\n"
            "行，是把可遇不可求的短刃。\n");
        set("apply_weapon/secondhand sword", ([
            "intimidate": 50,
            "armor_vs_fire": 50,
            "armor_vs_ice": 50,
            "armor_vs_wind": 50,
            "armor_vs_lightning": 50,
        ]));
        set("apply_weapon/sword", ([
            "intimidate": 25,
            "armor_vs_fire": 25,
            "armor_vs_ice": 25,
            "armor_vs_wind": 25,
            "armor_vs_lightning": 25,
        ]));
    }
    setup();
}
