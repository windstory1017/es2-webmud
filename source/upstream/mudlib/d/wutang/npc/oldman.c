// /d/wutang/npc/oldman.c — 五堂鎮 NPC：老人（依五堂鎮設計表）。

// 蝶影幻步任務（使用者提供舊資料，2026-11-07；設計見 docs/martial_arts/瑯夷派_蝶影幻步.md）：
// 玩家走進後院三次，老人請玩家幫忙；玩家說「好」或點頭答應後，去暗巷找李山，
// 回來把金牌交給老人，老人給一條小紅巾，帶去給廟祝。進度存在玩家身上，不限時間，
// 其他人插進來也各自解各自的任務。

#include <npc.h>
#include "/d/wutang/butterfly_quest.h"
inherit F_VILLAGER;

// 碰面三次的對白（第三次接著請託）。
string *MEET_MSG = ({
    "$N露出和藹的微笑。",
    "$N說道﹕年輕人..我們又見面了呀。",
    "$N說道﹕年輕人..我們倆還真是有緣呀 !",
    "$N微笑著說道﹕ 年輕人你幫我這個老頭子作一件事好嗎 ?",
});
string *ACCEPT_MSG = ({
    "$N說道﹕請你去市集小路旁的巷子內，對著巷子內的那個人提起前兒日子的事。",
    "$N說道﹕他便會引你去拿，到時請您再拿還給我。",
});
string *PLAQUE_MSG = ({
    "$N很高興的說道﹕謝謝你啦，年輕人。",
    "$N小心地將金牌收好。",
    "$N說道﹕這樣好了，你帶著這個紅巾來找廟祝，他會給你東西的。",
});

void create()
{
    seteuid(getuid());
    set_name("老人", ({ "oldman", "old man", "man" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 1200);
    set("long", @LONG
一位慈眉善目的老人，笑吟吟的看著你。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "老人坐在石椅上，瞇著眼睛曬太陽，看起來十分愜意。\n",
        "老人笑吟吟地說：「年輕人，常來這後院坐坐，對身子骨好啊。」\n",
    }));
    set_power("C");
    setup();
    carry_object("/obj/area/obj/cloth")->wear();
}

// 依序說完一串話，最後呼叫 done（沒有就不呼叫）。每個玩家各自一串，互不干擾。
void speak(object who, string *lines, int i, string done)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    bq_say(this_object(), who, lines[i]);
    if( i + 1 < sizeof(lines) ) call_out("speak", BQ_TICK, who, lines, i + 1, done);
    else if( done ) call_out(done, BQ_TICK, who);
}

// 每次走進後院算碰面一次；碰面第三次老人開口請託。
void init()
{
    object who = this_player();
    int n;

    ::init();
    if( !userp(who) || !living(this_object()) || is_fighting() ) return;

    switch( bq_stage(who) ) {
    case BQ_NONE:
        n = who->query(BQ_MEET) + 1;
        if( n < 3 ) {
            who->set(BQ_MEET, n);
            call_out("speak", BQ_TICK, who, n == 1 ? MEET_MSG[0..1] : ({ MEET_MSG[2] }), 0, 0);
        } else {
            who->delete(BQ_MEET);
            who->set(BQ_STAGE, BQ_ASKED);
            call_out("speak", BQ_TICK, who, ({ MEET_MSG[3] }), 0, 0);
        }
        break;
    case BQ_SCARF:
        // 小紅巾弄丟了（賣掉、丟了）：再給一條，免得任務卡死。
        if( !bq_find(who, BQ_SCARF_FILE) )
            call_out("regive", BQ_TICK, who);
        break;
    }
}

void regive(object who)
{
    if( !objectp(who) || environment(who) != environment() || bq_find(who, BQ_SCARF_FILE) ) return;
    bq_say(this_object(), who, "$N說道﹕小紅巾弄丟了嗎﹖別擔心，再給你一條。");
    bq_give(who, BQ_SCARF_FILE);
    message_vision(CYN "$N交給$n一條小紅巾。\n" NOR, this_object(), who);
}

// 玩家回答請託：說「好」「可以」「是」之類的肯定話。
void relay_say(object who, string msg)
{
    if( !userp(who) || bq_stage(who) != BQ_ASKED || !bq_positive(msg) ) return;
    who->set(BQ_STAGE, BQ_ACCEPTED);
    call_out("speak", BQ_TICK, who, ACCEPT_MSG, 0, 0);
}

// 對老人點頭（nod）也算答應。
void relay_emote(object who, string verb)
{
    if( !userp(who) || verb != "nod" || bq_stage(who) != BQ_ASKED ) return;
    who->set(BQ_STAGE, BQ_ACCEPTED);
    call_out("speak", BQ_TICK, who, ACCEPT_MSG, 0, 0);
}

// 收下玩家自己找到的金牌，回報小紅巾。
int accept_object(object who, object ob)
{
    if( !userp(who) || bq_stage(who) != BQ_PLAQUE || !bq_mine(who, ob, BQ_PLAQUE_FILE) ) return 0;
    who->set(BQ_STAGE, BQ_SCARF);
    call_out("speak", BQ_TICK, who, PLAQUE_MSG, 0, "give_scarf");
    return 1;
}

void give_scarf(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    bq_give(who, BQ_SCARF_FILE);
    message_vision(CYN "$N交給$n一條小紅巾。\n" NOR, this_object(), who);
}
