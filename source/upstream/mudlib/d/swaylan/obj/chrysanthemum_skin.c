// /d/swaylan/obj/chrysanthemum_skin.c — 菊花露袋（五陵客棧店小二販售），買到時裝 3 公升菊花露。

inherit CONTAINER_ITEM;

// 液體 volume 單位：1000 = 1 升
#define SKIN_CAPACITY 3000

void create()
{
    seteuid(getuid());
    set_name("菊花露袋", ({ "skin" }));
    set_weight(300);
    set_max_encumbrance(SKIN_CAPACITY);
    set("liquid_container", 1);
    if( !clonep() ) {
        set("unit", "個");
        set("value", 80);
        set("long", "一只縫得細密的皮袋，袋口用細繩紮緊，袋身上用墨畫了一朵菊花。\n");
    }
    setup();
    if( clonep() ) {
        object w = new("/d/swaylan/obj/chrysanthemum_dew");
        w->set_volume(SKIN_CAPACITY);
        w->move(this_object());
    }
}
