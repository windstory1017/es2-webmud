// /d/ziyan/obj/stone_key.c — 石鑰匙：按大堂三足金蟾的眼睛掉出來，打開大堂與內廳之間的木門。

inherit ITEM;

void create()
{
    set_name("石鑰匙", ({ "stone key", "key" }));
    set_weight(200);
    if( !clonep() ) {
        set("unit", "把");
        set("value", 0);
        set("long", "一把用青石雕成的鑰匙，入手沉甸甸的，齒紋刻得十分細緻。\n");
        set("lock", "ziyan_inner_hall");
    }
    setup();
}
