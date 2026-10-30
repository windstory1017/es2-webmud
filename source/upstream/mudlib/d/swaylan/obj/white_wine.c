// /d/swaylan/obj/white_wine.c — 甜菊花釀（五陵客棧店小二販售）：一只酒瓶，買到時裝 1 公升甜菊花釀。

inherit CONTAINER_ITEM;

#define BOTTLE_CAPACITY 1000

void create()
{
    seteuid(getuid());
    set_name("甜菊花釀", ({ "white wine", "wine", "bottle" }));
    set_weight(400);
    set_max_encumbrance(BOTTLE_CAPACITY);
    set("liquid_container", 1);
    if( !clonep() ) {
        set("unit", "個");
        set("value", 300);
        set("long", "一只粗陶燒成的酒瓶，瓶口塞著軟木塞，瓶身貼著一張紅紙，寫著「甜菊花釀」四字。\n");
    }
    setup();
    if( clonep() ) {
        object w = new("/d/swaylan/obj/chrysanthemum_wine");
        w->set_volume(BOTTLE_CAPACITY);
        w->move(this_object());
    }
}
