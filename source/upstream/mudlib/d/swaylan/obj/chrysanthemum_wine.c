// /d/swaylan/obj/chrysanthemum_wine.c — 甜菊花釀（裝在酒瓶裡）。

inherit LIQUID_ITEM;
inherit F_DRINK;

void create()
{
    set_name("甜菊花釀", ({ "chrysanthemum wine", "liquor" }));
    if( !clonep() ) {
        set("long", "用菊花與糯米釀成的甜酒，酒色清白，入口甘甜，後勁卻不小。\n");
        set("density", 1);
        set("alcohol", 0.2);
    }
    setup();
    set_volume(1000);
}
