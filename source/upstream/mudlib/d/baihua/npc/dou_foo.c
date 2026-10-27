// /d/baihua/npc/dou_foo.c — 百花村 NPC：杜福（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("杜福", ({ "dou foo", "dou", "foo" }));
    set_race("human");
    set_class("fighter");
    set_level(20);
    set("gender", "male");
    set("age", 40);
    set("long", @LONG
杜福是近幾年才搬進百花村定居的村民，平日沉默寡言，總是拄著一根長杖在崖邊散步。村裡的人都知道他棍法了得，曾經一個人打退過上山搶劫的盜匪，但問起他的來歷，他總是笑而不答。
LONG
    );
    set_power("B");
    setup();
    carry_object("/custom/weapon/staff/sinia_staff")->wield();
}
