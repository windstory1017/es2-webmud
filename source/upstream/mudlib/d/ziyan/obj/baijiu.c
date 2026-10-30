// /d/ziyan/obj/baijiu.c — 白干（裝在白干酒瓶裡）。

inherit LIQUID_ITEM;
inherit F_DRINK;

void create()
{
    set_name("白干", ({ "baijiu", "liquor" }));
    if( !clonep() ) {
        set("long", "辛辣嗆鼻的高粱白干，一口下肚，從喉嚨一路燒到肚子裡。\n");
        set("density", 1);
        set("alcohol", 0.4);
    }
    setup();
    set_volume(1000);
}
