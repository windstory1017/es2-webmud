/* 玄晦戰甲 — ES2 原始護具資料。所屬 NPC 另行設定。 */
#include <ansi.h>
#include <armor.h>

inherit F_ARMOR;
// 原資料「可以讀內功」：用 study 研讀，基本內功最高讀到 100。
// 閱讀條件（使用者定案，2026-11-03）：要先有基本內功（1 級以上）才能研讀。
inherit F_STUDY;

void create()
{
    set_name("\x1b[1;30m玄晦戰甲\x1b[m", ({ "black harness", "harness" }));
    set_weight(5000);
    setup_armor();

    if( !clonep() ) {
        set("unit", "件");
        set("value", 250000);
        set("long",
            "這是一副黑色有如長袍的甲冑，摸起來非絲非革，而且出乎意料之外的沉重。\n"
            "甲冑上刻著許多密密麻麻的古文。\n");
        set("content", ([ "force": 100 ]));
        set("required/skill", ([ "force": 1 ]));
        set("wear_as", "armor");
        set("apply_armor/armor", ([
            "cps": 2,
            "force": 10,
            "armor": 25,
        ]));
    }
    setup();
}

/* 沒有基本內功時，用戰甲自己的說法拒絕（使用者定案，2026-11-03），不用書本共用的「不足以閱讀這本書」。 */
int study_ob(object me)
{
    if( me->query_skill("force", 1) < 1 ) {
        tell_object(me, "你無法理解玄晦戰甲上記載的內容。\n");
        return 1;
    }
    return study::study_ob(me);
}
