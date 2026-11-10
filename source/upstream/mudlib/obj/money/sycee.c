// sycee.c
// 金元寶：一錠值一百兩黃金（1,000,000 文）。
// 設計文件：docs/CURRENCY.md（使用者指示，2026-11-07）

inherit MONEY;

void create()
{
	set_name("金元寶", ({"sycee", "sycee_money"}));
	if( !clonep() ) {
		set("money_id", "sycee");
		set("long", @LONG
一錠足色的金元寶﹐兩頭翹起、中間隆起﹐底下鏨著錢莊的戳記﹐
掂在手裡沉甸甸的。
LONG
		);
		set("unit", "些");
		set("material", "gold");
		set("base_value", 1000000 );
		set("base_unit", "錠");
		set("base_weight", 0); // CUSTOM: money is weightless
	}
	set_amount(1);
	setup();
}
