// /d/wutang/obj/golden_plaque.c — 金牌（蝶影幻步任務道具，設計見 docs/martial_arts/瑯夷派_蝶影幻步.md）。
// 記著是誰找到的（quest_owner）；別人拿去交給老人不算數。價值 10000。
inherit ITEM;

void create()
{
    set_name("金牌", ({ "golden plaque", "plaque", "plate" }));
    set_weight(500);
    if( !clonep() ) {
        set("unit", "面");
        set("value", 10000);
        set("long", "幾年前五堂鎮鄉里為了感謝鎮天神的保佑，特別籌鑄了這塊金牌謝神。\n");
    }
    setup();
}
