// /d/lee/obj/snow_dagger.c
// NEW（李家村復原版新增道具，非原始 ES2 資料，由專案成員指定數值）
// 匕首類武器上限：倍率(x)<=3、威力加成(z)<=100（比劍類的 5/150 低）。
// 原始需求 x=5/z=120% 超過匕首上限，經確認後改用上限值 x=3/z=100%；
// 連帶地最低傷害也從原本推算的 10 降到 8（見下方 setup_dagger 的 y=16 換算）。

#include <weapon.h>
#include <ansi.h>

inherit F_DAGGER;

void create()
{
    set_name(RED "雪魂匕" NOR, ({ "snow dagger", "dagger" }));
    set_weight(5000);
    setup_dagger(3, 16, 100, 5);   // 傷害基礎 8~53（另加力量加成，上限+48）

    if( !clonep() ) {
        set("wield_as", ({ "dagger", "secondhand dagger" }));
        set("unit", "把");
        set("value", 10000);
        set("rigidity", 25);
        set("long", "匕身寒光流轉，鋒刃薄如蟬翼，森冷的寒意逼人。\n");
        set("wield_msg", "$N抽出一把雪魂匕握在手中。\n");
        // 裝備時附加：根骨+1、傷害力+5（單手、副手皆適用）
        set("apply_weapon/dagger", (["con": 1, "damage": 5]));
        set("apply_weapon/secondhand dagger", (["con": 1, "damage": 5]));
    }
    setup();
}
