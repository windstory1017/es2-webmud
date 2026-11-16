/* 青色道袍 — 白雲觀天風道長的道袍（使用者設計表，2026-11-16）。 */
#include <ansi.h>
#include <armor.h>

inherit F_CLOTH;

void create()
{
    set_name("青色道袍", ({ "cyan taoist robe", "taoist robe", "robe" }));
    set_weight(1200);
    setup_cloth();

    if( !clonep() ) {
        set("unit", "件");
        set("value", 30000);
        set("long",
            "一件青色的道袍，布料細密，衣襟上繡著隱隱的雲紋，穿在身上隱約能抵擋火、風、冰、雷四種術法的侵襲。\n");
        set("wear_as", "cloth");
        set("apply_armor/cloth", ([
            "armor": 5,
            "armor_vs_fire": 20,
            "armor_vs_wind": 20,
            "armor_vs_ice": 20,
            "armor_vs_lightning": 20,
            "wis": 1,
        ]));
    }
    setup();
}
