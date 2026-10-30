// /d/swaylan/temple/obj/dabei_robe.c — 大悲法衣（寶蓮寺住持無想禪師的衣服，依水嵐縣設計表）。
// 設計表另有「大悲咒 10」，遊戲裡還沒有這個技能，先不放（docs/AREA_SWAYLAN.md 第三批）。
#include <armor.h>

inherit F_CLOTH;

void create()
{
    set_name("大悲法衣", ({ "dabei robe", "robe" }));
    set_weight(2000);
    setup_cloth();

    if( !clonep() ) {
        set("unit", "件");
        set("value", 6000);
        set("long",
            "一件素白的法衣，衣襟上以淡金絲線繡著千手觀音的法相，千手舒展，\n"
            "掌心各有一眼，彷彿垂視著世間眾生的苦難。穿在身上，心頭便覺一片\n"
            "清涼寧定。\n");
        set("wear_as", "cloth");
        set("apply_armor/cloth", ([
            "armor": 2,
            "spi": 1,
            "int": 1,
        ]));
    }
    setup();
}
