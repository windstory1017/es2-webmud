// /d/wutang/npc/ro.c — 五堂鎮 NPC：老駱（依五堂鎮設計表）。

#include <npc.h>
inherit F_VILLAGER;
inherit "/d/wutang/npc/ferryman";   // 羿水擺渡（使用者指示，2026-10-30）

void create()
{
    seteuid(getuid());
    set_name("老駱", ({ "shipman", "ro", "ferryman ro" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 55);
    set("long", @LONG
一位在此討生活多年的船夫。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    set("boat_room", "/d/wutang/ferry_boat_ro");
    // 攬客：約 5～7 tick 說一句（每 tick 15% 機率，與其他 NPC 相同）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "老駱拍了拍船舷，扯開嗓子吆喝：「過河嘍！過河只要五十文錢，穩穩當當送您到對岸！」\n",
    }));
    set_power("C");
    setup();
}
