// /d/ziyan/obj/juice.c — 桑柿汁（裝在果汁瓶裡）。

inherit LIQUID_ITEM;
inherit F_DRINK;

void create()
{
    set_name("桑柿汁", ({ "juice" }));
    if( !clonep() ) {
        set("long", "紫煙小棧的特產，用桑葚與柿子榨成的果汁，顏色紫紅，酸中帶甜。\n");
        set("density", 1);
    }
    setup();
    set_volume(1000);
}
