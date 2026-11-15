// /d/baobao/obj/skeleton.c — 枯乾的骸骨：鬼境的擺設，樣子與屍體腐化後的骸骨一樣，但不會消失。

inherit ITEM;

void create()
{
    set_name("枯乾的骸骨", ({ "skeleton" }));
    if( !clonep() ) {
        set("unit", "具");
        set("long", "這副骸骨已經躺在這裡很久了。\n");
        set("no_get", "死者為大，還是別去搬動遺體吧。\n");
    }
    setup();
}
