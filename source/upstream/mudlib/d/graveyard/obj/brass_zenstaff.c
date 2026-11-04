// /d/graveyard/obj/brass_zenstaff.c — 黃銅禪杖（亂葬崗的老和尚所用，依亂葬崗設計表）。
// 以 /custom/weapon/staff/zenstaff.c 為基礎，重量與價格乘 1.2，另加慧根 1、守勢等級 5。
#include <ansi.h>
#include <weapon.h>

inherit F_STAFF;

void create()
{
    set_name("黃銅禪杖", ({ "brass zenstaff", "zenstaff", "staff" }));
    set_weight(18840);
    init_damage(3, 18, 135, 0, "twohanded staff");

    if( !clonep() ) {
        set("wield_as", "twohanded staff");
        set("affix_short_name", "黃銅禪杖");
        set("unit", "把");
        set("value", 72000);
        set("long",
            "一把黃銅鑄成的禪杖，杖頭套著幾枚銅環，拿在手中相當沉重。\n");
        set("apply_weapon/twohanded staff", ([
            "wis": 1,
            "wittiness": 5,
        ]));
    }
    setup();
    // 武器附加屬性：每把新產生的武器擲一次前綴／後綴（adm/daemons/enhanced.c），同禪杖。
    if( clonep() ) ENHANCE_D->roll_affix(this_object());
}
