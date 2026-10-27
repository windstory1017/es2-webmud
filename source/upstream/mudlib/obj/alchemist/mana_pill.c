// 龍圖獨立丹藥；只依賴 mudlib 通用核心，不載入其他丹藥或 HEAT 專用檔案。
// produce_param 為方案 B；服用邏輯完整寫於本檔。
inherit COMBINED_ITEM;
void create()
{
    set_name("醒神丹", ({"mana pill", "pill"}));
    if (!clonep()) {
        set("long", @LONG
醒神丹是一種提神醒腦、恢復神思的丹藥，由遠志、石菖蒲、茯神、人蔘及甘草等數種藥材煉
製而成。此丹具有寧神益智之效，能使因過度耗神而疲憊的精神迅速恢復，為方士常用的丹藥
之一。
LONG
	);
	        set("unit", "些");
        	set("base_unit", "粒");
	        set("base_weight", 5);
        	set("produce_param", ([
	            "min_heat_power" : 130,
        	    "max_heat_power" : 150,
	            "min_heat" : 450,
	            "max_heat" : 491,
        	]));
	        set("base_value", 3000);
        set("mixture_value", 600);
    }
    set("requirement", (["literate": 45, "alchemy-medication": 40]));
    set("pill_grade", 2);   // 丹藥階級：一階最高、三階最低（使用者指示 #7，2026-10-23）
    set_amount(1);
    setup();
}

int stuff_ob(object me)
{
	if (me->query_stat_maximum("food") - me->query_stat("food") < 8)
		return notify_fail(gender_self(me) + "的肚子已經撐得連這粒藥丸也吞不下去了。\n");
	message_vision("$N吞下一粒醒神丹。\n", me);
	me->consume_stat("water", 10);
	me->supplement_stat("food", 5);
	me->heal_stat("sen", 15 + random(6));
	me->supplement_stat("sen", 30 + random(6));
	me->consume_stat("sen", 8);
	me->damage_stat("gin", 1);
	me->consume_stat("gin", 5);
	add_amount(-1);

	return 1;
}


// 供煉丹系統辨識；單獨使用時無需煉丹系統。
string query_heat_product() { return "mana"; }

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
