// /d/swaylan/forest/npc/bandit_chief.c — 密林 NPC：大頭目（依水嵐縣設計表第五批，docs/AREA_SWAYLAN.md）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("大頭目", ({ "bandit leader", "leader", "chief" }));
    set_race("dingling");
    set_class("fighter");
    set_level(20);
    set("age", 32);
    set("long", @LONG
一個身材矮壯的釘靈漢子，管著底下好幾隊盜賊，臉上一道刀疤從額頭斜斜劃到嘴角，說起官兵便破口大罵。
LONG
    );
    set_skill("sword", 40);
    set_skill("dodge", 40);
    set_skill("parry", 40);
    set_skill("unarmed", 40);
    // 跟著大統領一起罵官兵與這鳥地方；約 5～7 tick 說一句。
    set("chat_chance", 15);
    set("chat_msg", ({
        "大頭目呸了一口：「那些當兵的，除了欺負老百姓還會什麼！」\n",
        "大頭目罵道：「這鳥地方，連罈像樣的酒都沒有！」\n",
        "大頭目跟著大統領吼道：「官兵來一個殺一個，來兩個殺一雙！」\n",
    }));
    set("bounty", ([ "military service": 150 ]));
    set_power("A");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/d/swaylan/forest/obj/jade_armor")->wear();
    carry_money("silver", 5 + random(26));
}
