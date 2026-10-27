// 龍圖獨立丹藥；只依賴 mudlib 通用核心，不載入其他丹藥或 HEAT 專用檔案。
// produce_param 為方案 B；服用邏輯完整寫於本檔。
#include <ansi.h>

inherit COMBINED_ITEM;

void create()
{
	set_name("大還丹", ({"major heal pill", "major", "pill" }));
	if( !clonep() ) {
		set("unit", "些");
		set("long", 
"大還丹是一種極神奇的仙丹, 具強大的療效可以讓服用者的\n"
"內外傷在瞬間癒合, 不過煉製大還丹所需的藥材除了小還丹\n"
"所需的幾樣藥材外, 還必需找尋雪蓮, 千年人蔘等數種罕見\n"
"的珍貴藥材。因此, 大還丹除了燒煉困難外, 藥材之價格亦\n"
"極昂貴。\n");
		set("base_unit", "粒");
		set("produce_param", ([
            "min_heat_power" : 460,
            "max_heat_power" : 500,
            "min_heat" : 1900,
            "max_heat" : 2100,
        ]));
        set("base_value", 30000);
		set("base_weight", 5);
		set("mixture_value", 50000);
	}
	set("requirement", (["literate": 65, "alchemy-medication": 90]));
	set("pill_grade", 1);   // 丹藥階級：一階最高、三階最低（使用者指示 #7，2026-10-23）
    set_amount(1);
	setup();
}

int stuff_ob(object me)
{
	if (me->query_stat_maximum("food") - me->query_stat("food") < 10)
		return notify_fail(gender_self(me) + "的肚子已經撐得連這粒藥丸也吞不下去了。\n");
	message_vision("$N吞下一粒大還丹。\n", me);
	me->consume_stat("water", 18);
	me->supplement_stat("food", 7);
	me->heal_stat("kee", 70);
	me->supplement_stat("kee", 100);
	me->heal_stat("HP", 10);
	me->supplement_stat("HP", 25);
	add_amount(-1);

	return 1;
}

// 供煉丹系統辨識；單獨使用時無需煉丹系統。
string query_heat_product() { return "major"; }

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
