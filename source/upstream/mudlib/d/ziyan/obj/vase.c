// /d/ziyan/obj/vase.c — 果汁瓶（紫煙小棧販售）：買到時裝大約五升桑柿汁，敘述照設計表原文。

inherit CONTAINER_ITEM;

#define VASE_CAPACITY 5000

void create()
{
    seteuid(getuid());
    set_name("果汁瓶", ({ "vase" }));
    set_weight(500);
    set_max_encumbrance(VASE_CAPACITY);
    set("liquid_container", 1);
    if( !clonep() ) {
        set("unit", "個");
        set("value", 400);
        set("long", "一個普通的瓶子, 裡面裝著紫煙小棧的特產『桑柿汁』.\n");
    }
    setup();
    if( clonep() ) {
        object w = new("/d/ziyan/obj/juice");
        w->set_volume(VASE_CAPACITY);
        w->move(this_object());
    }
}
