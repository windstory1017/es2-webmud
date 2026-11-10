// nugget.c
// 復刻紀念金元寶：一錠值一百錠金元寶（100,000,000 文）。
// 錢莊可以兌換、提領，活動也可以當獎勵發放。
// 設計文件：docs/CURRENCY.md（使用者指示，2026-11-07）

inherit MONEY;

void create()
{
	set_name("復刻紀念金元寶", ({"nugget", "nugget_money"}));
	if( !clonep() ) {
		set("money_id", "nugget");
		set("long", @LONG
這是一錠特大的金元寶﹐照著古時的樣式重新鑄造﹐金光燦燦﹐
底部鏨著「復刻紀念」四個字。
LONG
		);
		set("unit", "些");
		set("material", "gold");
		set("base_value", 100000000 );
		set("base_unit", "錠");
		set("base_weight", 0); // CUSTOM: money is weightless
	}
	set_amount(1);
	setup();
}
