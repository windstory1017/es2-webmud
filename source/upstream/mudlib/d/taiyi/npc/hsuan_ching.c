// /d/taiyi/npc/hsuan_ching.c — 太乙觀：玄清道長（Hsuan-ching，依太乙觀設計表，2026-11-09）。
/*
白齊的得意門徒（使用者更正，2026-11-09），45 歲。有謎題，會傳授太乙劍法（taiyie），
設計見 docs/martial_arts/道士共通_太乙劍法.md：
  對他 emote sigh（嘆氣）→ 他困惑，轉頭與你談山崖的景色 → 你說「獨上高樓望盡天涯路」→ 傳授。
只有道士（含茅山派）可學；每人只傳一次。
*/
#include <ansi.h>

#define TAUGHT   "taiyie/taught"
#define WAITING  "riddle_waiting"
#define LINE     "獨上高樓望盡天涯路"
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("玄清道長", ({ "hsuan-ching", "taoist", "玄清道長" }));
    set_race("human");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 45);
    set("title", "太乙觀道長");
    set("long", @LONG
一位身著青衣道袍的道長，眉宇間帶著幾分落拓不羈，神情總像在思索著什麼。他是太乙觀觀主白齊的得意門徒，手中那柄桃木劍看似尋常，卻隱隱透著一股深不可測的氣度。
LONG
    );
    set("custom_faction", "taoist.freeze");   // 素衣派（docs/martial_arts/門派武功對照表.md）
    set_skill("taiyie", 100);
    map_skill("sword", "taiyie");
    map_skill("parry", "taiyie");
    set("chat_chance_combat", 50);
    set("chat_msg_combat", ({
        (: command, "cast 1" :),
        (: command, "cast 2" :),
        (: command, "cast 3" :),
        (: command, "cast 4" :)
    }));
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
    carry_object("/custom/armor/cloth/green_robe")->wear();   // 青衣道袍
}

// ------------------------------------------------------------------
// 謎題：emote sigh → 轉頭談景 → say 獨上高樓望盡天涯路 → 傳授太乙劍法
// 敘述一句一行，不自動斷行（使用者指示，2026-11-09）。
// ------------------------------------------------------------------

private int talk_gap() { return (1 + random(2)) * 2; }

private int still_here(object who)
{
    return objectp(who) && environment(who) == environment();
}

private void speak(string msg)
{
    message_vision(msg, this_object(), 0, 1);
}

int accept_info(object ob, string type)
{
    return type == "skills";
}

// 玩家對他做表情動作；嘆氣（sigh）才算。
void relay_emote(object who, string verb)
{
    if( verb != "sigh" ) return;
    if( !objectp(who) || query_temp(WAITING) ) return;   // 正在和別人說話
    if( who->query(TAUGHT) ) return;
    if( who->query_class() != "taoist" ) {
        call_out("not_for_you", 1, who);
        return;
    }
    set_temp(WAITING, who);
    call_out("confused", 1, who);
}

void not_for_you(object who)
{
    if( !still_here(who) ) return;
    speak("$N瞥了" + who->name() + "一眼﹐淡淡說道﹕這套劍法與你無緣。\n");
}

void confused(object who)
{
    if( !still_here(who) ) { delete_temp(WAITING); return; }
    speak("$N微微一愣﹐不解地望了" + who->name() + "一眼﹐隨即轉過頭去﹐望向遠方。\n");
    call_out("scenery1", talk_gap(), who);
}

void scenery1(object who)
{
    if( !still_here(who) ) { delete_temp(WAITING); return; }
    speak("$N輕聲說道﹕此處山色如畫﹐雲海翻湧﹐令人心胸為之一寬。\n");
    call_out("scenery2", talk_gap(), who);
}

void scenery2(object who)
{
    if( !still_here(who) ) { delete_temp(WAITING); return; }
    speak("$N又道﹕可惜登高望遠﹐總覺得少了一句話能道盡此情此景。\n");
    call_out("give_up", 60, who);   // 一直沒人接話就不等了
}

void give_up(object who)
{
    if( query_temp(WAITING) == who ) delete_temp(WAITING);
}

// 玩家說話：標點與空白不計，完整說出那句詩才算。
void relay_say(object who, string arg)
{
    string s;

    if( query_temp(WAITING) != who || !stringp(arg) ) return;
    s = arg;
    foreach(string p in ({ "，", ",", "﹐", "。", "！", "!", "﹗", "、", " ", "　", "？", "?" }))
        s = replace_string(s, p, "");
    if( strsrch(s, LINE) == -1 ) return;
    delete_temp(WAITING);
    remove_call_out("give_up");
    call_out("teach1", 1, who);
}

void teach1(object who)
{
    if( !still_here(who) ) return;
    speak("$N眼睛一亮﹐撫掌大笑﹐說道﹕好一句「獨上高樓望盡天涯路」！\n");
    call_out("teach2", talk_gap(), who);
}

void teach2(object who)
{
    if( !still_here(who) ) return;
    speak("$N說道﹕貧道正是在此處悟出一套劍法﹐今日便傳與有緣人。\n");
    call_out("teach3", talk_gap(), who);
}

void teach3(object who)
{
    string sk = "taiyie";

    if( !still_here(who) ) return;
    speak("$N將太乙劍法從頭演示了一遍﹐" + who->name() + "在一旁細心領會。\n");
    if( who->query(TAUGHT) ) return;
    who->set(TAUGHT, 1);
    if( who->query_learn(sk) < who->skill_threshold(sk, 1) )
        who->improve_skill_exact(sk, who->skill_threshold(sk, 1) - who->query_learn(sk));
    tell_object(who, HIY "你學會了太乙劍法。\n" NOR);
}
