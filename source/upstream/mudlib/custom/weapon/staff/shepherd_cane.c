/* 牧羊杖 — 百花村牧童的杖（使用者設計表，2026-10-20）。
 * 強度同短劍（obj/area/obj/shortsword），單手杖，價值 1000 文。 */
#include <weapon.h>

inherit F_STAFF;

void create()
{
    set_name("牧羊杖", ({ "cane", "staff" }));
    set_weight(3000);
    init_damage(1, 15, 40, 1, "staff");

    if( !clonep() ) {
        set("wield_as", "staff");
        set("unit", "根");
        set("value", 1000);
        set("long",
            "一根細長的竹杖，頂端彎成鉤狀，是放牧時用來驅趕羊群的。杖身繫著一條\n"
            "褪了色的紅布條，看得出主人十分愛惜。\n");
    }
    setup();
    // 武器附加屬性：每把新產生的武器擲一次前綴／後綴（adm/daemons/enhanced.c）。
    if( clonep() ) ENHANCE_D->roll_affix(this_object());
}
