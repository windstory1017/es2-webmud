// /d/baobao/obj/corpse.c — 腐爛的男屍：鬼境的擺設，樣子與屍體腐化後的男屍一樣，但不會變成骸骨。

inherit ITEM;

void create()
{
    set_name("腐爛的男屍", ({ "corpse" }));
    if( !clonep() ) {
        set("unit", "具");
        set("long", "這具屍體顯然已經躺在這裡有一段時間了﹐正散發著一股腐屍的味道。\n");
        set("no_get", "死者為大，還是別去搬動遺體吧。\n");
    }
    setup();
}
