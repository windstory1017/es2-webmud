// snake_powder.c — 肥遺骨粉（使用者定案，2026-10-21）。王燕販賣。
// 吃下去氣味太衝﹐反而吐了出來：食物 −20、飲水 −30。

inherit COMBINED_ITEM;
inherit F_FOOD;

void create()
{
	set_name("肥遺骨粉", ({ "snake powder", "powder" }) );
	if( !clonep() ) {
		set("unit", "些");
		set("base_unit", "包");
		set("base_value", 1500);
		set("base_weight", 50);
		set("long", "一小包灰白色的粉末﹐據說是用肥遺的骨頭研磨而成﹐湊近一聞﹐一股腥臭直衝腦門。\n");
	}
	setup();
}

int stuff_ob(object me)
{
	message_vision("$N吃下一包肥遺骨粉。\n", me);
	tell_object(me, "強烈的氣味讓你的胃腸一陣翻攪﹐不自主「噁～」的吐了一大口。\n");
	tell_room(environment(me), me->name() + "臉色一變﹐「噁～」的一聲吐了一大口。\n", ({ me }));
	me->consume_stat("food", 20);
	me->consume_stat("water", 30);
	add_amount(-1);
	return 1;
}
