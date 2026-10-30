// /d/ziyan/obj/bloody_letter.c — 血書：在紫煙鎮民宅的書櫃裡找到（敘述使用者定稿，2026-10-30）。

inherit ITEM;

void create()
{
    set_name("血書", ({ "bloody letter", "letter" }));
    set_weight(50);
    if( !clonep() ) {
        set("unit", "封");
        set("value", 0);
        set("long", "上面看起來似字非字、又不像是什麼有意義的符號。\n");
    }
    setup();
}
