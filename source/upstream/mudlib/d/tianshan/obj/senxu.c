// /d/tianshan/obj/senxu.c — 天山野蔘鬚（Senxu）。桃仙身上帶著。
// 吃下後：飽食度 +25；氣的目前值 +100、實格（上限）也恢復 100；形體（HP）目前值 +20。
// 價值 20000、重量 10（使用者指示，2026-11-16）。都不會超過上限。

inherit ITEM;

void create()
{
    set_name("天山野蔘鬚", ({ "senxu", "ginseng whisker", "whisker" }));
    set_weight(10);
    if( !clonep() ) {
        set("unit", "束");
        set("value", 20000);
        set("long", "一小束從老山蔘根上摘下的細鬚，色澤微黃，帶著淡淡的藥香與一絲桃花的清氣。據說只有天山迷霧深處的老蔘才養得出這樣的蔘鬚，嚼在口中先苦後甘，暖意久久不散。\n");
    }
    setup();
}

int stuff_ob(object me)
{
    if( me->query_stat_maximum("food") - me->query_stat("food") < 25 )
        return notify_fail("你的肚子已經撐得什麼東西也吞不下去了。\n");

    message_vision("$N將天山野蔘鬚放入口中細細嚼了嚼﹐一股清涼的暖意順著喉頭化開﹐散入四肢百骸。\n", me);
    me->supplement_stat("food", 25);
    me->heal_stat("kee", 100);
    me->supplement_stat("kee", 100);
    me->supplement_stat("HP", 20);
    destruct(this_object());
    return 1;
}
