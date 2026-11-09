// pot.c

inherit ITEM;

void create()
{
	object water;

	seteuid(getuid());
	set_name("大水缸", ({ "pot" }));
	set_max_encumbrance(300000);
	set("long", "一個裝滿清水的大水缸﹐如果你口渴﹐可以舀水來喝(drink)。\n");
	set("no_get", 1);
	set("liquid_container", 1);
	setup();
	if( clonep() ) {
		water = new("/obj/water");
		water->set_volume(150000);
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

// 補滿：清水被喝光時液體物件會消失，以前的 reset() 只補還在的，喝乾後就不會再有。
// 現在不見了就重新放一份，再補到滿；房間每次重生（reset）時也會呼叫。
void refill()
{
	object liquid;

	if( !(liquid = present("water", this_object())) ) {
		seteuid(getuid());
		liquid = new("/obj/water");
		liquid->move(this_object());
	}
	liquid->set_volume(150000);
}

void reset()
{
	refill();
}
