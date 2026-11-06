// /d/snowkeep/obj/brass_key.c — 黃銅鑰匙：魚鐵山身上的地牢鑰匙，開地牢（H2）往地牢深處（J2）的牢門。
// 設計見 docs/AREA_SNOWKEEP.md。

inherit ITEM;

void create()
{
    set_name("黃銅鑰匙", ({ "brass key", "key" }));
    set_weight(50);
    if( clonep() )
        set_default_object(__FILE__);
    else {
        set("unit", "把");
        set("long", "一把沉甸甸的黃銅鑰匙﹐齒紋粗大﹐看來是用來開某道厚重的鐵鎖。\n");
        set("value", 10);
        set("lock", "snowkeep_dungeon");   // 開地牢牢門（d/snowkeep/dungeon.c）
    }
}
