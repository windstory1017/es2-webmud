// /d/shuiyue/obj/tea_pot.c — 水月村十字路口的茶水桶：固定在房間裡、裝 100 升清水，喝完會補滿。
// 做法同 d/lee/obj/well.c。

inherit ITEM;

#define POT_VOLUME 100000   // 液體 volume 單位：1000 = 1 升，100000 = 100 升

void create()
{
    object water;

    seteuid(getuid());   // 產生時要放清水，需要有效 UID 才能 new()
    set_name("茶水桶", ({ "tea pot", "pot", "bucket" }));
    set_max_encumbrance(180000);
    set("long", "一只裝滿清水的大木桶，桶旁懸著一支竹勺，口渴的人可以舀水來喝 (drink)。\n");
    set("no_get", 1);
    set("liquid_container", 1);
    setup();
    if( clonep() ) {
        water = new("/obj/water");
        water->set_volume(POT_VOLUME);
        water->move(this_object());
    }
}

varargs int accept_object(object me, object ob)
{
    if( ob )
        if( !userp(ob) )
            return 1;
    else return notify_fail("你不能將玩家放到容器裡面。\n");
}

// 補滿：清水被喝光時液體物件會消失，不見了就重新放一份，再補到滿。
void refill()
{
    object liquid;

    if( !(liquid = present("water", this_object())) ) {
        seteuid(getuid());
        liquid = new("/obj/water");
        liquid->move(this_object());
    }
    liquid->set_volume(POT_VOLUME);
}

void reset()
{
    refill();
}
