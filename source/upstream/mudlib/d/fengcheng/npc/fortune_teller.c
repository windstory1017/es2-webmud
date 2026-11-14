// /d/fengcheng/npc/fortune_teller.c — 風城 NPC：神算子（D16 交叉路口）。
// 用法與喬陰縣算命先生（d/qiaoyin/npc/fortune_teller.c）相同，差別是只告知這項技能的發展是否已到極限。
//
// 算天賦（使用者指定，2026-10-23；設計見 docs/FORTUNE_TELLER.md）：
//   ask teller about 算命 → 開價一兩黃金 → give 錢 → inquire <技能> → 私下告訴玩家這項技能是否已到個人上限。

#include <ansi.h>
#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("神算子", ({ "fortune teller", "teller" }));
    set_race("human");
    set_class("alchemist");
    set_level(10);
    set("gender", "male");
    set("age", 35);
    set("long", @LONG
面容清瘦，帶著一種看透世事的微笑，桌上擺著龜殼與銅錢，等待想探知自身前程的人上門。
LONG
    );
    set_power("C");
    setup();
    carry_object("/obj/cloth")->wear();
}

#define FEE          10000   // 一卦一兩黃金（等值貨幣、多付不找零）
#define WAIT_SECONDS 60      // 付錢後在原地等候的時間：30 tick（1 tick 約 2 秒）
#define TICK         2       // 1 tick 約 2 秒，對話之間的間隔

void init()
{
    ::init();
    add_action("do_inquire", "inquire");
}

private string title_of(object who) { return who->rank("respectful"); }

void accept_inquiry(object who, string key)
{
    string t;

    if( key != "算命" ) return;
    t = title_of(who);
    message_vision("$N微笑說道﹕呵呵？這位" + t + "想算命吧，老夫一見到你就知道是有緣人。\n",
        this_object(), who);
    call_out("say_to", 2 * TICK, who, t + "且不用開口，嗯 ... 想問事業？");
    call_out("say_to", 3 * TICK, who, "這樣吧，就當作和這位" + t + "結個緣，一卦只要一兩黃金，如何？");
}

// 延後說的話：對方已離開就不說了。
void say_to(object who, string msg)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say " + msg);
}

// 付錢：一兩黃金算一卦。付了之後離開這裡就算放棄；在這裡待太久也作廢。都不退錢。
int accept_object(object who, object ob)
{
    int token;

    if( !ob->query("money_id") ) return 0;
    if( who->query_temp("fortune_teller/paid") ) {
        command("say 卦金方才已經收過了，想問哪方面的事業，儘管說吧。");
        return 0;
    }
    if( ob->value() < FEE ) {
        command("say 一卦一兩黃金，少一文也算不得準。");
        return 0;
    }
    token = time();
    who->set_temp("fortune_teller/paid", token);
    // 等「你拿出…交給神算子」的訊息先出現，再開始占卜。
    call_out("start_reading", 0, who, token);
    call_out("fate_timeout", WAIT_SECONDS, who, token);
    return 1;
}

void start_reading(object who, int token)
{
    if( !objectp(who) || who->query_temp("fortune_teller/paid") != token ) return;
    message_vision("$N微笑著向$n點了點頭，問明了$n的生辰八字，拿出一個龜殼和幾枚銅錢，占卜了起來。\n",
        this_object(), who);
    message_vision("$N問道﹕不知" + title_of(who) + "要問 (inquire) 的是哪方面的事業？\n", this_object());
}

private void forfeit(object who) { who->delete_temp("fortune_teller/paid"); }

// 付了錢卻遲遲不問：還在這裡就說一句，離開了就默默作廢。
void fate_timeout(object who, int token)
{
    if( !objectp(who) || who->query_temp("fortune_teller/paid") != token ) return;
    forfeit(who);
    if( environment(who) == environment() )
        command("say " + title_of(who) + "既然拿不定主意，看來緣分未到，改日再來吧。");
}

// 依英文代碼或中文名稱，找出玩家身上的技能；沒有就傳回 0。
private string find_skill(object who, string arg)
{
    mapping skills = who->query_skills();
    string sk;

    if( !mapp(skills) ) return 0;
    foreach(sk in keys(skills))
        if( skills[sk] > 0 && (sk == arg || to_chinese(sk) == arg) ) return sk;
    return 0;
}

int do_inquire(string arg)
{
    object who = this_player();
    string sk, t;

    if( !who->query_temp("fortune_teller/paid") ) return 0;
    // 付了錢之後離開這裡就算放棄。
    if( environment(who) != environment() ) { forfeit(who); return 0; }
    if( !arg || arg == "" ) {
        command("say " + title_of(who) + "想問哪方面的事業，還請明說。");
        return 1;
    }
    t = title_of(who);
    if( !(sk = find_skill(who, arg)) ) {
        command("say 嗯 ... 看來" + t + "並沒有這項技能。");
        return 1;
    }
    forfeit(who);
    message_vision("$N將龜殼中的銅錢搖了搖，灑在桌上，然後掐指算了算。\n", this_object());
    call_out("tell_fate", 2 * TICK, who, sk, t);
    return 1;
}

// 算完了才私下告訴對方（只有對方看得到）：這項技能是否已到個人上限。
void tell_fate(object who, string sk, string t)
{
    string name, words;

    if( !objectp(who) ) return;
    name = to_chinese(sk);
    if( who->query_skill(sk, 1) >= who->query_personal_skill_cap(sk) )
        words = "上的發展已經到了極限。";
    else
        words = "上還可以繼續發展。";
    tell_object(who, CYN "神算子告訴你﹕" + t + "在" + name + words + "\n" NOR);
}
