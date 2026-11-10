// /d/wutang/npc/lee_shan.c — 五堂鎮 NPC：李山（依五堂鎮設計表）。

#include <npc.h>
#include "/d/wutang/butterfly_quest.h"
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("李山", ({ "lee shan", "lee", "shan" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 25);
    set("long", @LONG
附近遊手好閒的小混混，時不時就會做出些令人苦惱的爛事。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "李山蹲在牆角，賊頭賊腦地東張西望，好像在提防著什麼人。\n",
        "李山不耐煩地踢了踢地上的石子：「看什麼看？沒見過人在這裡乘涼嗎？」\n",
    }));
    set_power("C");
    setup();
}

// 蝶影幻步任務（使用者提供舊資料，2026-11-07）：老人要玩家來暗巷對李山提起前兒日子的事，
// 李山就說出金牌藏在民房後的小溪。每個玩家各自進行，進度存在玩家身上。
void speak(object who, int i)
{
    string *lines = ({
        "$N很驚呀的說道﹕這...這...怎麼可能....",
        "$N嘆了一口氣",
        "$N說道﹕好吧..我認了..那個東西我藏在民房後的小溪中。",
    });

    if( !objectp(who) || environment(who) != environment() ) return;
    bq_say(this_object(), who, lines[i]);
    if( i + 1 < sizeof(lines) ) call_out("speak", BQ_TICK, who, i + 1);
    else if( bq_stage(who) == BQ_ACCEPTED ) who->set(BQ_STAGE, BQ_LEE);
}

void relay_say(object who, string msg)
{
    if( !userp(who) || bq_stage(who) != BQ_ACCEPTED || !stringp(msg) ) return;
    if( strsrch(msg, "前兒日子的事") == -1 ) return;
    call_out("speak", BQ_TICK, who, 0);
}
