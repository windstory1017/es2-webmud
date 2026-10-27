// hockshop_storage.c

inherit ROOM;

void create()
{
	set("short", "當鋪的共用倉庫");
	set("long", "這裡是某間當鋪的儲藏室﹐你不該出現在這裡。\n");
	setup();
}

int clean_up()
{
	// Do not clean up ourself unless the shop is cleaned up.
	if( objectp(query("shop_ob")) ) return 1;
	destruct(this_object());
	return 0;
}

// 當鋪連線後不再有「放久了改上架」的機制（docs/PAWNSHOP_NETWORK.md，2026-10-23）。
// 原本的寫法把 pawned_time 打成 pwaned_time，這個機制從未生效。
void reset() { }
