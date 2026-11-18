// /d/ziyan/obj/bloody_letter.c — 血書：在紫煙鎮民宅的書櫃裡找到。
// 敘述改為字跡模糊、只認得「趙老賊」「勾結」「冤」（使用者指示，2026-11-18）。
// 玄冰烈焰斧任務的信物，交給茅屋的程槐，設計見 docs/martial_arts/玄冰烈焰斧_任務設計.md。

inherit ITEM;

void create()
{
    set_name("血書", ({ "bloody letter", "letter" }));
    set_weight(50);
    if( !clonep() ) {
        set("unit", "封");
        set("value", 0);
        set("long", "一封寫在粗布上的血書，字跡被血漬暈得模糊不清，只勉強認得出「趙老賊」\n"
                    "「勾結」「冤」幾個字，其餘都糊成了一片暗紅。\n");
    }
    setup();
}
