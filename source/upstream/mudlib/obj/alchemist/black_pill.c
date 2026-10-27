// 龍圖獨立丹藥；只依賴 mudlib 通用核心，不載入其他丹藥或 HEAT 專用檔案。
// produce_param 為方案 B；服用邏輯完整寫於本檔。
inherit COMBINED_ITEM;

void create()
{
	set_name("烏心丹", ({"black pill", "pill"}));
	if( !clonep() ) {
		set("unit", "些");
		set("long",
			"烏心丹是一種民間很普遍的藥方﹐用來恢復因操練、工作所耗的體力\n"
			"有不錯的效果﹐缺點是吃了以後會昏昏沈沈想睡覺﹐且容易口渴。\n");
		set("base_unit", "粒");
		set("base_value", 80);
		set("base_weight", 5);
		
		set("mixture_value", 30);
		
		set("produce_param", ([
            "min_heat_power" : 80,
            "max_heat_power" : 250,
            "min_heat" : 100,
            "max_heat" : 150,
        ]));
		
		set("requirement", (["literate": 1]));
		set("pill_grade", 3);   // 丹藥階級：一階最高、三階最低（使用者指示 #7，2026-10-23）
	}
	set_amount(1);
	setup();
}

int stuff_ob(object me)
{
	if( me->query_stat_maximum("food") - me->query_stat("food") < 5 )
		return notify_fail(gender_self(me) + "的肚子已經撐得連這粒藥丸也吞不下去了。\n");
	message_vision("$N吞下一粒黑黑的藥丸。\n", me);
	me->consume_stat("water", 30);
	me->supplement_stat("food", 5);
	me->damage_stat("sen", 1);
	me->consume_stat("sen", 4);
	me->supplement_stat("gin", 15);
	me->supplement_stat("kee", 15);
	add_amount(-1);

	return 1;
}

// 供煉丹系統辨識；單獨使用時無需煉丹系統。
string query_heat_product() { return "black"; }

// 僅合併同一新物件檔案的堆疊，不與舊丹藥建立關聯。
varargs int move(mixed dest, int silent)
{
    object other;
    if (!::move(dest, silent)) return 0;
    foreach (other in all_inventory(environment())) {
        if (other != this_object() && base_name(other) == base_name(this_object())) {
            add_amount(other->query_amount());
            destruct(other);
        }
    }
    return 1;
}
