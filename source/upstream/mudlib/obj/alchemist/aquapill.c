// 龍圖獨立丹藥；只依賴 mudlib 通用核心，不載入其他丹藥或 HEAT 專用檔案。
// produce_param 為方案 B；服用邏輯完整寫於本檔。
inherit COMBINED_ITEM;

void create()
{
	set_name("金液丹", ({"aquapill", "pill"}));
	if( !clonep() ) {
		set("unit", "些");
		set("long",
			"金液丹是一種方家丹藥的總稱，主要的功能是療傷，金液丹的療傷\n"
			"功效相當不錯，而且所需的藥材很容易買到，但是因為煉製過程比\n"
			"較需要技術，所以只有大城市的藥舖才買得到。\n");
		set("base_unit", "粒");
		set("base_value", 160);
		set("base_weight", 5);
		set("mixture_value", 50);
		set("produce_param", ([
            "min_heat_power" : 90,
            "max_heat_power" : 180,
            "min_heat" : 420,
            "max_heat" : 540,
        ]));
		
		set("requirement", (["literate": 10, "alchemy-medication": 30]));
		set("pill_grade", 3);   // 丹藥階級：一階最高、三階最低（使用者指示 #7，2026-10-23）
	}
	set_amount(1);
	setup();
}

int stuff_ob(object me)
{
	if( me->query_stat_maximum("food") - me->query_stat("food") < 20)
		return notify_fail(gender_self(me) + "的肚子已經撐得連這粒藥丸也吞不下去了。\n");
	message_vision("$N吞下一粒金液丹。\n", me);
	me->consume_stat("water", 10);
	
	me->supplement_stat("food", 5);
	me->damage_stat("gin", 3);
	me->consume_stat("gin", 5);
	me->heal_stat("kee", 25);
	me->supplement_stat("kee", 25);
	add_amount(-1);

	return 1;
}

// 供煉丹系統辨識；單獨使用時無需煉丹系統。
string query_heat_product() { return "aqua"; }

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
