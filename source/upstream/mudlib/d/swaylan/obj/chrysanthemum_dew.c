// /d/swaylan/obj/chrysanthemum_dew.c — 菊花露（裝在菊花露袋裡）。

inherit LIQUID_ITEM;
inherit F_DRINK;

void create()
{
    set_name("菊花露", ({ "chrysanthemum dew", "dew" }));
    if( !clonep() ) {
        set("long", "用菊花泡出來的清涼茶水，色澤淡黃，入口微甘，帶著淡淡的花香。\n");
        set("density", 1);
    }
    setup();
    set_volume(1000);
}
