// /d/swaylan/forest/npc/bandit_captain.c — 密林 NPC：大統領（依水嵐縣設計表第五批，docs/AREA_SWAYLAN.md）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("大統領", ({ "bandit captain", "captain" }));
    set_race("headless");
    set_class("fighter");
    set_level(40);
    set("age", 35);
    set("long", @LONG
一個身形魁梧的形天，雙手各握一柄大斧，渾身筋肉虯結，一開口便是震耳欲聾的大嗓門。據說他與振武軍的魯熙年有不共戴天之仇。
LONG
    );
    set_skill("axe", 80);
    set_skill("secondhand axe", 80);
    set_skill("dodge", 80);
    set_skill("parry", 80);
    set_skill("unarmed", 80);
    // 罵魯熙年、官兵與這鳥地方；約 5～7 tick 說一句。
    set("chat_chance", 15);
    set("chat_msg", ({
        "大統領一斧劈在桌上：「魯熙年那個王八蛋，總有一天老子要親手剁了他！」\n",
        "大統領罵道：「官兵算什麼東西，有種就進林子裡來！」\n",
        "大統領恨恨地說：「老子堂堂好漢，竟要龜縮在這鳥地方！」\n",
    }));
    set("bounty", ([ "military service": 750 ]));
    set_power("A");
    setup();
    carry_object("/custom/weapon/axe/large_axe")->wield();
    carry_object("/custom/weapon/axe/dragon_tiger_axe")->wield("secondhand axe");   // 青龍白虎斧
    carry_money("silver", 5 + random(26));
}
