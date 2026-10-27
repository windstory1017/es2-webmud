// /d/wutang/npc/keeper.c — 五堂鎮 NPC：廟祝（依五堂鎮設計表）。
//
// 超渡（使用者指定，2026-10-24）：魂魄狀態的玩家對廟祝說「投胎」，廟祝二次確認；
// 30 秒內說「確認」（「確定」「是」「要」「好」或對廟祝點頭 nod 也算，2026-10-24），廟祝唸經超渡，玩家魂飛魄散，回到選種族（業力結算同鬼魂再死一次，
// 見 docs/業力系統.md：CHAR_D->make_mist() → LOGIN_D->reincarnate()）。

#include <ansi.h>
#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("廟祝", ({ "keeper" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("age", 40);
    set("long", @LONG
鎮天神廟的廟祝，平日管理廟裡的大小事，最近正為了丟失廟裡的重要東西而發愁。
LONG
    );
    set_skill("unarmed", 2);
    set_skill("dodge", 2);
    set_skill("parry", 2);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "廟祝望著神桌嘆了口氣：「那東西要是找不回來，我可怎麼向鎮天神交代啊……」\n",
        "廟祝拿著雞毛撢子撣了撣香爐，嘴裡唸唸有詞，不知道在祈求些什麼。\n",
    }));
    set_power("C");
    setup();
    carry_object("/obj/area/obj/cloth")->wear();
}

#define CONFIRM_TIME 30
// 回答廟祝的二次確認時，說這些話開頭都算同意。
#define CONFIRM_WORDS ({ "確認", "確定", "是", "要", "好" })

private int is_soul(object who)
{
    return objectp(who) && userp(who) && who->query("life_form") == "ghost";
}

// 廟祝問過之後 CONFIRM_TIME 秒內同意，才開始超渡。
private void confirm(object who)
{
    int asked = who->query_temp("wutang_keeper/reincarnate");

    if( !asked || time() - asked > CONFIRM_TIME ) return;
    who->delete_temp("wutang_keeper/reincarnate");
    call_out("release_soul", 1, who);
}

void relay_say(object who, string msg)
{
    if( !is_soul(who) || !stringp(msg) ) return;

    if( strsrch(msg, "投胎") == 0 ) {
        who->set_temp("wutang_keeper/reincarnate", time());
        command("say 施主確定要這樣嗎？施主尚可復生重回陽間。");
        return;
    }
    foreach(string w in CONFIRM_WORDS)
        if( strsrch(msg, w) == 0 ) { confirm(who); return; }
}

// 對廟祝點頭（nod keeper）也算同意。
void relay_emote(object who, string verb)
{
    if( verb == "nod" && is_soul(who) ) confirm(who);
}

// 唸經超渡：鬼魂消散（訊息同鬼魂再死一次），然後回到選種族。
void release_soul(object who)
{
    if( !is_soul(who) || environment(who) != environment() ) return;

    message_vision("$N開始唸著經文替$n超渡。\n", this_object(), who);
    tell_object(who, "\n你化做一股輕煙，消失了。\n" HIR "你已經魂飛魄散了！！！\n" NOR);
    message("vision", who->name() + "化成一陣煙，消失了 ...\n", environment(who), who);
    CHAR_D->make_mist(who);
}
