inherit LIQUID_ITEM;
inherit F_DRINK;
void create()
{
    set_name("天靈清露", ({ "heaven dew", "dew" }));
    if( !clonep() ) {
        set("long", "澄澈清香的露液。原版商品名稱已確認，但精確數值效果尚未找回；目前只提供標準飲用行為，不額外杜撰特殊增益。\n");
        set("density", 1);
        set("base_value", 32);   // 每單位 32 文 × 500 = 16000 文（使用者定案，2026-10-22）
    }
    setup();
    set_volume(500);
}
