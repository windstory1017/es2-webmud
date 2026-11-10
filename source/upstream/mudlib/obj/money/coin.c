// coin.c

inherit MONEY;

void create()
{
	set_name("錢", ({"coin", "coins", "coin_money" }));
	if( !clonep() ) {
		set("money_id", "coin");
		set("long", @LONG
看起來外圓內方的扁平金屬。
LONG
		);  // 使用者指示，2026-11-07
		set("unit", "些");
		set("material", "copper");
		set("base_value", 1);
		set("base_unit", "文");
		set("base_weight", 0); // CUSTOM: money is weightless
	}
	set_amount(1);
	setup();
}

