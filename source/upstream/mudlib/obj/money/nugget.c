// nugget.c
// 復刻紀念金元寶：一錠值五百錠金元寶（500,000,000 文）。
// 錢莊只收不付：可以存入、可以換成其他錢幣，但不能提領、也不能用其他錢幣換成紀念金元寶；
// 活動可以當獎勵發放。名稱以亮黃色顯示。
// 設計文件：docs/CURRENCY.md（使用者指示，2026-11-07；2026-11-16 修訂）

#include <ansi.h>

inherit MONEY;

void create()
{
	set_name(HIY "復刻紀念金元寶" NOR, ({"nugget", "nugget_money"}));
	if( !clonep() ) {
		set("money_id", "nugget");
		set("long", @LONG
這是一錠特大的金元寶﹐照著古時的樣式重新鑄造﹐金光燦燦﹐
底部鏨著「復刻紀念」四個字。
LONG
		);
		set("unit", "些");
		set("material", "gold");
		set("base_value", 500000000 );
		set("base_unit", "錠");
		set("base_weight", 0); // CUSTOM: money is weightless
	}
	set_amount(1);
	setup();
}
