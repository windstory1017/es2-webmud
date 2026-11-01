// /d/wutang/npc/huang.c — 五堂鎮 NPC：小黃（依五堂鎮設計表）。

#include <npc.h>
#include <ansi.h>
inherit F_VILLAGER;
inherit "/d/wutang/npc/ferryman";   // 羿水擺渡（使用者指示，2026-10-30）

int together;   // 與老駱同在一岸的 tick 數

/* 老駱與小黃同在一岸時，過 3 tick 小黃就撐船到對岸候著（使用者指示，2026-10-30）。 */
void check_partner()
{
    object env = environment(), ro;

    call_out("check_partner", 2);
    if( !env || is_crossing() || !other_bank() ) { together = 0; return; }
    ro = present("ferryman ro", env);
    if( !objectp(ro) || ro->is_crossing() ) { together = 0; return; }
    if( ++together < 3 ) return;
    together = 0;
    // 換岸訊息用青色（使用者定稿，2026-10-30）。
    cross_alone(CYN "$N笑道：「駱伯，我到對岸候著」說罷便跳上小船，撐篙往對岸去了。\n" NOR,
        "一條小船從對岸緩緩靠了過來，$N跳上岸，把纜繩往木樁上一繫。\n");
}

void create()
{
    seteuid(getuid());
    set_name("小黃", ({ "shipman", "huang" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 15);
    set("long", @LONG
一位年輕氣壯、精力旺盛的小伙子。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    set("boat_room", "/d/wutang/ferry_boat_huang");
    // 攬客：約 5～7 tick 說一句（每 tick 15% 機率，與其他 NPC 相同）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "小黃朝著路過的人招手：「客倌要過河嗎？五十文錢就好，一眨眼就到對岸啦！」\n",
    }));
    set_power("C");
    setup();
    if( clonep() ) call_out("check_partner", 2);
}
