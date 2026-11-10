// /custom/legacy_npc/wu_changuei.c — 舊站 NPC：吳長貴（Wu changuei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在亂葬崗密林（/d/graveyard/forest_w）。
瑯夷派師父（使用者定案，2026-11-04）：任何未加入門派的盜賊都可拜師，不限種族、性別；
拜師後成為「瑯夷派弟子」。拜師信物是瞎眼老太婆給的古錢（d/snow/npc/obj/coin.c）：
把古錢交給吳長貴、承認是祖奶奶叫你來的，才能拜師（使用者定案，2026-11-04）。瑯夷派內功見 docs/martial_arts/瑯夷派_瑯夷派內功.md。
舊站區域：未能判定；房間：—（2026-11-04 放入亂葬崗）。
尚未製作的裝備（先空著）：點鋼匕(Steel dagger)、襯甲(Padded armor)。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "thief.longye"

void create()
{
    seteuid(getuid());
    set_name("吳長貴", ({ "wu changuei", "wu", "changuei" }));
    set("nickname", "無常鬼");
    set_race("human");
    set_class("thief");
    set_level(15);
    set("gender", "male");
    set("age", 35);
    set("long",
        "一個面皮白淨的高瘦漢子﹐左頰上一道長長的刀疤﹐你突然想起這個\n"
        "漢子就是這一帶著名的飛賊「無常鬼」吳長貴。\n");
    set("custom_faction", "thief.longye");   // 瑯夷派（docs/martial_arts/門派武功對照表.md）
    set_skill("stealing", 45);   // 傳授偷竊用；門派自動技能沒有偷竊（2026-11-06）
    set_power("B");
    setup();
    carry_object("/obj/area/obj/dagger")->wield();   // 匕首
    carry_object("/custom/armor/head/ghost_mask")->wear();   // 鬼面
    carry_object("/custom/armor/cloth/black_suit")->wear();   // 黑獄服
}

// ------------------------------------------------------------------
// 信物：把瞎眼老太婆給的古錢交給吳長貴（使用者定案，2026-11-04）。
// 1. hmm  2. 問是不是祖奶奶叫你來的  3. 玩家說是或點頭  4. 「好吧」（從此可以拜師）
// 5～8. 叮嚀幾句。每句話之間隔 1～2 回合（一回合兩秒）。
// ------------------------------------------------------------------

#define TOKEN_MARK "longye/token"     // 玩家身上的記號：已經憑古錢得到吳長貴認可
#define ASKING     "longye/asking"    // 吳長貴暫存：正在等誰回答

private string *advice = ({
    "我也沒什麼能教你的﹐這些把戲只能算是行走江湖的小伎倆。",
    "我們瑯夷派說不上行俠仗義﹐但也不做違背良心的事情。",
    "若讓我知道你做了什麼傷天害理之事﹐就不要怪我了…",
    "好了好了…去吧。",
});

private int talk_gap() { return (1 + random(2)) * 2; }

private int still_here(object who)
{
    return objectp(who) && environment(who) == environment();
}

void start_hmm(object who)
{
    command("hmm");
    call_out("ask_token", talk_gap(), who);
}

void ask_token(object who)
{
    if( !still_here(who) ) { delete_temp(ASKING); return; }
    command("say 是祖奶奶叫你來的？");
    set_temp(ASKING, who);
    call_out("give_up", 60, who);   // 一直不回答就不等了，免得擋住別人
}

void give_up(object who)
{
    if( query_temp(ASKING) == who ) delete_temp(ASKING);
}

void say_advice(object who, int i)
{
    if( !still_here(who) || i >= sizeof(advice) ) return;
    command("say " + advice[i]);
    if( i + 1 < sizeof(advice) ) call_out("say_advice", talk_gap(), who, i + 1);
}

void approve(object who)
{
    if( !still_here(who) ) return;
    command("say 好吧。");
    who->set(TOKEN_MARK, 1);
    call_out("say_advice", talk_gap(), who, 0);
}

int accept_object(object who, object ob)
{
    if( !ob->query("thief_token") ) return 0;
    if( query_temp(ASKING) ) return 0;   // 別人正在交古錢：直接不收（使用者定案，2026-11-04）
    set_temp(ASKING, 1);   // 問話之前也算忙著，避免兩人同時交古錢
    call_out("start_hmm", 1, who);   // 等「你拿出古錢交給吳長貴」那行先出現
    call_out("drop_token", 1, ob);   // give 指令會直接銷毀有價物品；萬一還在就收走
    return 1;
}

void drop_token(object ob)
{
    if( objectp(ob) ) destruct(ob);
}

/* 玩家回答：說「是」「對」「沒錯」「正是」「嗯」之類（不含「不是」「沒有」）都算肯定。 */
private int positive(string arg)
{
    if( !stringp(arg) ) return 0;
    if( strsrch(arg, "不") != -1 || strsrch(arg, "沒有") != -1 ) return 0;
    foreach(string w in ({ "是", "對", "沒錯", "正是", "嗯", "恩", "yes", "Yes" }))
        if( strsrch(arg, w) != -1 ) return 1;
    return 0;
}

private void answered(object who)
{
    delete_temp(ASKING);
    call_out("approve", talk_gap(), who);
}

void relay_say(object who, string arg)
{
    if( query_temp(ASKING) != who ) return;
    if( positive(arg) ) answered(who);
}

void relay_emote(object who, string verb)
{
    if( query_temp(ASKING) != who ) return;
    if( verb == "nod" ) answered(who);
}

// ------------------------------------------------------------------
// 拜師：任何未加入門派的盜賊都收，不限種族、性別；要先憑古錢得到認可；拜師後成為瑯夷派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    string faction = me->query("custom_faction"), title = me->query("title");

    if( me->query_class() != "thief" ) {
        command("say 咱們瑯夷派只收道上混飯吃的兄弟﹐你還是請回吧。");
        return 0;
    }
    if( (stringp(faction) && faction != "" && faction != FACTION)
    ||  (stringp(title) && strsrch(title, "隱教") != -1) ) {
        command("say 你已經拜了別家的山頭﹐我吳長貴可不搶人。");
        return 0;
    }
    if( faction != FACTION && !me->query(TOKEN_MARK) ) {
        command("say 我跟你非親非故的﹐憑什麼收你？");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set("title", "瑯夷派弟子");
        me->set("custom_faction", FACTION);
        do_chat("吳長貴點了點頭﹐從今往後你就是瑯夷派的弟子了。\n");
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：瑯夷派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// 背刺、潛行、偷竊、匕首、閃躲、招架、瑯夷派內功、內功（人物等級 5 起）；殺手本能不教。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "backstab":        // 使用者定案，2026-11-06（殺手本能不教）
    case "sneak":
    case "stealing":
    case "dagger":
    case "dodge":
    case "parry":
        break;
    case "longyeforce":
    case "force":
        if( me->query_level() < 5 )
            return notify_fail("吳長貴搖頭道：你的根基尚淺﹐還摸不透本門內功的門道。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("吳長貴說道：入門的訣竅都教給你了﹐往後自己去琢磨。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "吳長貴將「" + to_chinese(skill) + "」的入門訣竅傳授給你。\n" NOR);
    return 1;
}
