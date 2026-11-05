// /d/wutang/npc/yung_tai.c — 五堂鎮 NPC：雍泰（依五堂鎮設計表）。
//
// 風嘯拳法任務（使用者指示，2026-11-04～05，依舊資料 Quest 紀錄；設計見
// docs/martial_arts/共通_風嘯拳法.md「任務」）：
// - 所有職業都能做（使用者指示 2026-11-05，原本限武者）；已學過風嘯拳法（fonxiaoken）的人不再觸發。
// - 玩家走進醇雨樓客棧時，雍泰向玩家打聽失散的四弟。
// - 對他說「雍順」、交給他貓眼戒指，他會追問四弟下落。
// - 交過貓眼戒指之後再交骨灰罈：雍泰昏倒 5 tick，醒來後道謝並傳授風嘯拳法。
// - 每句間隔 1 tick（2 秒）；玩家中途離開就不再往下說。
//   傳授前玩家離開的話，下次回來雍泰會接著道謝、傳授。

#include <ansi.h>
#include <npc.h>
inherit F_FIGHTER;

#define TICK        2                   // 一個 tick 的秒數
#define FAINT_TICKS 5                   // 收下骨灰罈後昏倒幾個 tick
#define QUEST_SKILL "fonxiaoken"
#define RING        "/d/graveyard/obj/chrysoberyl_ring"
#define URN         "/d/graveyard/obj/urn"

// 進房時的對話，$N 是雍泰，$n 是剛進來的玩家。
string *GREET_MSG = ({
    "$N望了$n幾眼﹐忽然說道﹕這位朋友可是外地來的﹖\n",
    "$N接著說道﹕小弟想打聽一個人﹐是我那失散的四弟 ...\n",
    "$N說道﹕我那四弟個頭比我高些﹐但是瘦了點﹐前年在斐縣境中失散 ...\n",
    "$N嘆了口氣﹐說道﹕到現在一點消息也沒有﹐也不知道是生是死 ...\n",
    "$N說道﹕如果您有見到或聽到舍弟的消息﹐請千萬讓小弟知道 ...\n",
    "$N嘆了口氣﹐又繼續喝酒。\n",
});

string SAY_MSG  = "$N眼睛一亮﹐說道﹕雍順﹖正是舍弟﹗他在哪裡﹖若能告知小弟感激不盡 ...\n";
string RING_MSG = "$N一把搶過貓眼戒指﹐激動地說道﹕這正是舍弟的隨身之物﹗他 ... 他現在何方﹖\n";

// 醒來之後的道謝。從 THANK_OWED 那一句開始是「傳授前玩家離開，回來後接著說」的部分。
string *THANK_MSG = ({
    "$N捧著手中的骨灰罈﹐怔怔地流下淚來。\n",
    "$N說道﹕四弟 ... 沒想到連你也 ... 唉 ...\n",
    "$N說道﹕偌大的雍家現在只剩下我一個人了﹐四弟 ...\n",
    "$N嘆了口氣﹐小心地將骨灰罈包好收了起來。\n",
    "$N說道﹕這位朋友﹐小弟家道中落﹐沒有什麼貴重禮物足以報答 ...\n",
    "$N「嗯」的一聲﹐一副欲言又止的樣子。\n",
    "$N說道﹕這樣吧﹐小弟將這套家傳的拳法傳給恩公作為答謝之禮。\n",
});
#define THANK_OWED 4

int talking;                    // 正在說話（進房、道謝），其他人進來不插話
object mourner;                 // 交出骨灰罈、等雍泰醒來道謝的玩家

void create()
{
    seteuid(getuid());
    set_name("雍泰", ({ "yung tai", "yung", "tai" }));
    set_race("yenhold");
    set_class("fighter");
    set_level(20);
    set("age", 30);
    set("long", @LONG
雍泰是五堂鎮雍家這一代的老三﹐自從前年雍老爺從斐縣官期任滿
回鄉中途不幸被盜匪殺害﹐雍家四個兒子便流離各處﹐雍泰是幾個
月前才得到同鄉好友景襄仁景二爺幫助才得以湊足盤纏扶著老父棺
木回鄉安葬﹐雍家世代或為文官﹐或為武將﹐都是兩袖清風﹐家財
向來不富﹐但是卻相當受到五堂鎮居民的敬重﹐想不到這一代遭逢
厄運﹐眼看著就要沒落了。
LONG
    );
    set_skill("unarmed", 40);
    set_skill("dodge", 40);
    set_skill("parry", 40);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "雍泰獨自坐在角落喝著悶酒，望著窗外的鐵旗桿怔怔出神。\n",
        "雍泰嘆了口氣：「先父一生清廉，到頭來卻落得這般下場……」\n",
    }));
    set_power("B");
    setup();
}

// 任務對象：還沒學過風嘯拳法的玩家。
private int quest_player(object who)
{
    return objectp(who) && userp(who) && undefinedp(who->query_skills()[QUEST_SKILL]);
}

private int still_here(object who)
{
    return objectp(who) && environment(who) == environment()
        && living(this_object()) && !is_fighting();
}

// 說話時先停下平常的閒聊，免得插在對話中間。
private void start_talk() { talking = 1; set("chat_chance", 0); }
private void stop_talk()  { talking = 0; set("chat_chance", 15); }

private void say_line(string msg, object who)
{
    message_vision(CYN + msg + NOR, this_object(), who);
}

void init()
{
    object who = this_player();

    ::init();
    if( talking || mourner || !quest_player(who) || !living(this_object()) || is_fighting() )
        return;
    start_talk();
    if( who->query_temp("yung_tai/owed") )
        call_out("thank", TICK, who, THANK_OWED);
    else
        call_out("greet", TICK, who, 0);
}

void greet(object who, int step)
{
    if( !still_here(who) ) { stop_talk(); return; }
    say_line(GREET_MSG[step], who);
    if( ++step < sizeof(GREET_MSG) ) call_out("greet", TICK, who, step);
    else stop_talk();
}

// 對雍泰說「雍順」。
void relay_say(object who, string msg)
{
    if( mourner || !quest_player(who) || !stringp(msg) || strsrch(msg, "雍順") < 0 ) return;
    call_out("react", TICK, who, SAY_MSG);
}

void react(object who, string msg)
{
    if( still_here(who) ) say_line(msg, who);
}

int accept_object(object who, object ob)
{
    string file = base_name(ob);

    if( file != RING && file != URN ) return 0;
    if( mourner || !quest_player(who) || !living(this_object()) || is_fighting() ) return 0;

    if( file == RING ) {
        who->set_temp("yung_tai/ring", 1);
        call_out("react", TICK, who, RING_MSG);
        return 1;
    }

    // 骨灰罈：先交過貓眼戒指，雍泰才知道是四弟。
    if( !who->query_temp("yung_tai/ring") )
        return notify_fail("雍泰看了看骨灰罈﹐不明白你的意思﹐並不伸手去接。\n");
    who->delete_temp("yung_tai/ring");
    who->set_temp("yung_tai/owed", 1);
    mourner = who;
    remove_call_out("greet");
    start_talk();
    call_out("collapse", TICK);
    return 1;
}

void collapse()
{
    if( !living(this_object()) ) return;
    unconcious();
    // 昏倒 5 tick 後醒來（取代原本的甦醒時間）。
    remove_call_out("revive");
    call_out("revive", FAINT_TICKS * TICK);
}

varargs void revive(int quiet)
{
    object who = mourner;

    ::revive(quiet);
    mourner = 0;
    if( !objectp(who) ) { stop_talk(); return; }
    call_out("thank", TICK, who, 0);
}

void thank(object who, int step)
{
    if( !still_here(who) || !who->query_temp("yung_tai/owed") ) { stop_talk(); return; }
    say_line(THANK_MSG[step], who);
    if( ++step < sizeof(THANK_MSG) ) call_out("thank", TICK, who, step);
    else call_out("teach", TICK, who);
}

void teach(object who)
{
    stop_talk();
    if( !still_here(who) || !who->query_temp("yung_tai/owed") ) return;
    who->delete_temp("yung_tai/owed");
    if( !undefinedp(who->query_skills()[QUEST_SKILL]) ) return;
    who->improve_skill_exact(QUEST_SKILL, 1);
    tell_object(who, CYN "(雍泰將風嘯拳法的秘要盡數傳了給你)\n" NOR);
    message("vision", CYN + "雍泰低聲向" + who->name() + "講解了一套拳法的要訣。\n" NOR,
        environment(), who);
}
