/* 紫雲盔 — ES2 原始護具資料。所屬 NPC 另行設定。 */
#include <ansi.h>
#include <armor.h>

inherit F_HEAD_EQ;

void create()
{
    set_name("\x1b[1;35m紫雲盔\x1b[m", ({ "purple helmet", "helmet" }));
    set_weight(2000);
    setup_head_eq();

    if( !clonep() ) {
        set("unit", "頂");
        set("value", 12000);
        set("long",
            "一定造型優美﹐色調古怪的頭盔。看起來似乎很結實﹐不過多少給人\n"
            "一種不舒服的感覺。\n");
        set("wear_as", "head_eq");
        set("apply_armor/head_eq", ([
            "wis": 1,
            "spi": 1,
            "armor": 25,
        ]));
    }
    setup();
}
