/* 辛木杖 — 百花村杜福的兵器（使用者設計表，2026-10-20）。
 * 強度同木杖（long_staff），另加防禦力 5、防禦能力值 25；價值 100 兩銀子。 */
#include <ansi.h>
#include <weapon.h>

inherit F_STAFF;

void create()
{
    set_name("辛木杖", ({ "sinia staff", "staff" }));
    set_weight(12000);
    init_damage(3, 12, 135, 0, "twohanded staff");

    if( !clonep() ) {
        set("wield_as", "twohanded staff");
        set("unit", "把");
        set("value", 10000);
        set("long",
            "一根用辛木削成的長杖，木質堅韌，杖身被手掌磨得油亮，兩端還包著一圈\n"
            "鐵箍。拿在手中沉甸甸的，舞動起來虎虎生風，攻守之間都十分趁手。\n");
        set("apply_weapon/twohanded staff", ([
            "armor": 5,
            "defense": 25,
        ]));
    }
    setup();
    // 武器附加屬性：每把新產生的武器擲一次前綴／後綴（adm/daemons/enhanced.c）。
    if( clonep() ) ENHANCE_D->roll_affix(this_object());
}
