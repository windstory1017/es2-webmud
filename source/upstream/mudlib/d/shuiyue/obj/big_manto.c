// /d/shuiyue/obj/big_manto.c — 福安客棧賣的大饅頭。飽食 200，價值 20。

inherit COMBINED_ITEM;
inherit F_FOOD;

void create()
{
    set_name("大饅頭", ({ "big manto", "manto" }));
    if( !clonep() ) {
        set("unit", "堆");
        set("base_unit", "個");
        set("base_value", 20);
        set("value", 20);   // F_VENDOR 直接讀 value
        set("base_weight", 200);
        set("food_stuff", 200);
        set("long", "一個比拳頭大上好幾圈的白麵饅頭，蓬鬆結實，吃一個就很飽。\n");
    }
    set_amount(1);
    setup();
}
