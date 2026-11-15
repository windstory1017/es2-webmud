// /d/fengcheng/obj/offering.c — 供品：祭拜先祖時準備的食物，不能吃，純道具。價值 500 文。

inherit ITEM;

void create()
{
    set_name("供品", ({ "offering", "gong" }));
    set_weight(300);
    if( !clonep() ) {
        set("unit", "份");
        set("value", 500);
        set("long", "祭拜先祖時準備的食物。\n");
    }
    setup();
}
