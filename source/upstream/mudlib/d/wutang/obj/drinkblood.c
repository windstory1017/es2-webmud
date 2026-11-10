/* 飲血 — 蝶影幻步任務道具，同時是一把中等傷害的匕首（使用者定案，2026-11-07）。
 * 傷害力 5；力量 +1、膽識 +1；主手、副手匕首屬性與傷害一致。
 * 記著是誰拿到的（quest_owner）；別人拿去交給葉翔不算數。 */
#include <ansi.h>
#include <weapon.h>

inherit F_DAGGER;

void create()
{
    set_name("飲血", ({ "drinkblood", "dagger" }));
    set_weight(4000);
    init_damage(3, 8, 50, 5, "dagger");
    init_damage(3, 8, 50, 5, "secondhand dagger");

    if( !clonep() ) {
        set("wield_as", ({ "dagger", "secondhand dagger" }));
        set("unit", "把");
        set("value", 20000);
        set("long",
            "一把薄如蟬翼的匕首﹐刃口鋒利﹐在光下泛著一層淡淡的寒光。\n");
        set("apply_weapon/dagger", ([
            "str": 1,
            "cor": 1,
        ]));
        set("apply_weapon/secondhand dagger", ([
            "str": 1,
            "cor": 1,
        ]));
    }
    setup();
}
