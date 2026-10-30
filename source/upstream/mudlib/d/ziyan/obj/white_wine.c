// /d/ziyan/obj/white_wine.c — 白干酒（紫煙小棧販售）：一只酒瓶，買到時裝 1 公升白干。

inherit CONTAINER_ITEM;

#define BOTTLE_CAPACITY 1000

void create()
{
    seteuid(getuid());
    set_name("白干酒", ({ "white wine", "wine", "bottle" }));
    set_weight(400);
    set_max_encumbrance(BOTTLE_CAPACITY);
    set("liquid_container", 1);
    if( !clonep() ) {
        set("unit", "瓶");
        set("value", 400);
        set("long", "一只粗陶酒瓶，瓶口用紅布塞著，還沒打開就聞得到一股辛辣的酒氣。\n");
    }
    setup();
    if( clonep() ) {
        object w = new("/d/ziyan/obj/baijiu");
        w->set_volume(BOTTLE_CAPACITY);
        w->move(this_object());
    }
}
