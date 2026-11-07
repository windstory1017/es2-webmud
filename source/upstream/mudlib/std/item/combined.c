// combined_item.c

inherit ITEM;

// variables

int amount;

// implementations

int query_amount() { return amount; }

private void destruct_me() { destruct(this_object()); }

void set_amount(int v)
{
    if( v < 0 ) error("combine:set_amount less than 1.\n");
    amount = v;

    if( v==0 )	call_out("destruct_me", 0);
    else	this_object()->set_weight((int)(v * (float)query("base_weight")));
}

void add_amount(int v) { set_amount(amount+v); }

varargs string short(int raw)
{
    if( raw ) return ::short(raw);
    return chinese_number(query_amount()) + query("base_unit")
	+ ::short(raw);
}

varargs int move(mixed dest, int silent)
{
    if( ! ::move(dest, silent) ) return 0;

    // 放到人身上或房間地上時，與同一種物品合併成一堆（房間地上：使用者指示，2026-11-07）。
    // 袋子、屍體、丹爐等容器裡不合併，以免影響逐件計算的系統。
    if( environment()->is_character() || inherits(ROOM, environment()) )
    {
	object ob, env = environment();
	string file = base_name(this_object());

	foreach(ob in all_inventory(env))
	{
	    if( ob==this_object() ) continue;
	    if( base_name(ob)==file ) {
		amount += (int)ob->query_amount();
		// 被併掉的若是房間自己生成的物品，改由這一堆接手，房間重置時才不會再生一堆。
		if( function_exists("replace_spawn", env) ) env->replace_spawn(ob, this_object());
		destruct(ob);
	    }
	}
	set_amount(amount);
    }

    return 1;
}

int render_value()
{
    return to_int(query("base_value") * amount);
}

void setup()
{
    ::setup();
    if( !amount ) set_amount(1);
    set("value", (: render_value :));
}

