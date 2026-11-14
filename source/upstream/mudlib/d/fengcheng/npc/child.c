// /d/fengcheng/npc/child.c — 風城 NPC：字謎小孩（L18 小巷）。
//
// 猜字謎（舊資料「八寶樓謎語」的十題，使用者決定 2026-11-14）：
//   1. 玩家走進小巷，小孩問他要不要猜謎語。
//   2. 玩家用 say 說「好」「是」「沒問題」之類的話，或對小孩做正面的表情（點頭、微笑、鼓掌…）算同意。
//   3. 小孩隨機出一題「你說說看…是甚麼字？」，玩家用 say 說出答案。
//   4. 答對：小孩說出將軍府的秘密，並記下 quest/fengcheng_riddle，將軍府門衛從此不再攔人。
//   答錯可以重答；離開小巷再回來，小孩會重新問起。

#include <npc.h>

inherit F_VILLAGER;

// ({ 謎面, 答案 })，謎面與答案都來自舊資料。
private mixed *RIDDLES = ({
    ({ "點點螢火江邊照", "淡" }),
    ({ "飛砂走石", "少" }),
    ({ "弓上一枝箭，箭邊一把弓", "引" }),
    ({ "手無寸鐵", "控" }),
    ({ "半個月亮", "胖" }),
    ({ "半真半假", "值" }),
    ({ "一點不見", "視" }),
    ({ "十個哥哥", "克" }),
    ({ "日落半林中", "東" }),
    ({ "一月七日", "脂" }),
});

// 同意的說法與正面的表情。
private string *YES_WORDS = ({ "好", "是", "沒問題", "要", "可以", "行", "來", "願意", "當然", "ok", "OK", "yes" });
private string *YES_EMOTES = ({ "nod", "smile", "sm", "grin", "laugh", "applaud", "agree", "giggle", "ya",
    "addoil", "cong", "thank", "handshake", "hi", "hello", "wave", "bow", "courtesy", "hug", "pat", "wink" });

#define TICK 2   // 1 tick 約 2 秒

void create()
{
    seteuid(getuid());
    set_name("小孩", ({ "child" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 8);
    set("long", "一個蹲在牆根的小孩，手裡捏著一截石灰，眼睛骨碌碌地轉，一看就是滿肚子鬼點子。\n");
    set_skill("unarmed", 2);
    set_skill("dodge", 10);
    set_skill("parry", 2);
    set_power("C");
    setup();
}

// 玩家走進來：還沒猜對過的，小孩過一會兒問他要不要猜謎語。
void init()
{
    object who = this_player();

    ::init();
    if( !objectp(who) || !userp(who) || who->query("quest/fengcheng_riddle") ) return;
    who->delete_temp("fengcheng_riddle");
    who->set_temp("fengcheng_riddle/stage", "offer");
    call_out("offer", TICK, who);
}

void offer(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    if( who->query_temp("fengcheng_riddle/stage") != "offer" ) return;
    command("say " + who->name() + "，要不要來猜個謎語啊？");
}

// 出題。
void pose(object who)
{
    int n;

    if( !objectp(who) || environment(who) != environment() ) return;
    if( who->query_temp("fengcheng_riddle/stage") != "offer" ) return;
    n = random(sizeof(RIDDLES));
    who->set_temp("fengcheng_riddle/stage", "answer");
    who->set_temp("fengcheng_riddle/answer", RIDDLES[n][1]);
    command("emote 眼睛一亮，把手裡的石灰一丟");
    call_out("ask_riddle", TICK, who, RIDDLES[n][0]);
}

void ask_riddle(object who, string riddle)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say " + who->name() + "，你說說看" + riddle + "是甚麼字？");
}

void relay_emote(object who, string verb)
{
    if( who->query_temp("fengcheng_riddle/stage") != "offer" ) return;
    if( member_array(verb, YES_EMOTES) == -1 ) return;
    pose(who);
}

private int says_yes(string msg)
{
    string w;

    if( strsrch(msg, "不") >= 0 || strsrch(msg, "沒興趣") >= 0 ) return 0;
    foreach(w in YES_WORDS)
        if( strsrch(msg, w) >= 0 ) return 1;
    return 0;
}

void relay_say(object who, string msg)
{
    string stage = who->query_temp("fengcheng_riddle/stage");

    if( !stringp(stage) || !stringp(msg) ) return;
    if( stage == "offer" ) {
        if( says_yes(msg) ) pose(who);
        else if( strsrch(msg, "不") >= 0 ) {
            who->delete_temp("fengcheng_riddle");
            command("say 哼，不猜就不猜，沒膽子的。");
        }
        return;
    }
    if( stage != "answer" ) return;
    if( strsrch(msg, who->query_temp("fengcheng_riddle/answer")) >= 0 ) {
        who->delete_temp("fengcheng_riddle");
        who->set("quest/fengcheng_riddle", 1);
        command("emote 瞪大了眼睛，拍著手笑了起來");
        command("say 答對了！" + who->name() + "真聰明！");
        call_out("tell_secret", TICK, who);
    } else if( strlen(msg) <= 3 ) {
        // 只有說出單一個字才當成作答，避免玩家聊天時一直被回應。
        command("say 不對不對，再想想看。");
    }
}

void tell_secret(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("emote 左右張望了一下，湊到" + who->name() + "耳邊壓低了聲音");
    command("say 偷偷告訴你個秘密，將軍府裡藏了八個鬼喔。");
    call_out("reassure", TICK, who);
}

void reassure(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say 不過他們專愛欺負膽小的，你這麼機靈，去了也不用怕，門口的守衛不會再攔你啦。");
}
