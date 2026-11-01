// /d/swaylan/obj/noodle.c — 銀絲麵（五陵客棧店小二販售）。

inherit COMBINED_ITEM;
inherit F_FOOD;

void create()
{
    set_name("銀絲麵", ({ "swaylan noodle", "noodle" }));
    if( !clonep() ) {
        set("unit", "些");
        set("base_unit", "碗");
        set("base_value", 30);
        set("base_weight", 300);
        set("food_stuff", 100);
        set("long", "五陵客棧的招牌麵食，麵條拉得細如銀絲，浸在清亮的高湯裡，上頭撒著幾點蔥花，香氣撲鼻。\n");
    }
    setup();
}
