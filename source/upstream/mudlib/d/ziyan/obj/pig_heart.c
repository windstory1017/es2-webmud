// /d/ziyan/obj/pig_heart.c — 豬心（紫煙小棧販售）：食物 40、回氣 5，敘述照設計表原文。

inherit COMBINED_ITEM;
inherit F_FOOD;

void create()
{
    set_name("豬心", ({ "pig-heart", "heart" }));
    if( !clonep() ) {
        set("unit", "盤");
        set("base_unit", "片");
        set("base_value", 400);
        set("base_weight", 200);
        set("food_stuff", 40);
        set("heal_kee", 5);
        set("long", "一片赤紅的豬心, 上面還殘留著一些紅辣椒, 看起來非常可口, 據說是本店的祖傳調味秘方, 所以這片豬心不只好吃, 還具有一點療的效果.\n");
    }
    setup();
}
