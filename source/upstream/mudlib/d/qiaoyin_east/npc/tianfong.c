// /d/qiaoyin_east/npc/tianfong.c — 白雲觀：天風道長（Tianfong taoist，依使用者設計表，2026-11-16）。
/*
白雲觀觀主，玄衣派（taoist.storm）道士，巫首，180 歲。另闢蹊徑拆解天師四破，研究出防禦法術【紫薇心經】。
設計見 docs/AREA_QIAOYIN_EAST.md 與 docs/martial_arts/天師道法_紫薇心經.md。
謎題（使用者定案，2026-11-16）：職業是道士或方士、等級夠的玩家，在他面前
  say 紫薇心經，或 ask taoist about 紫薇心經 → 他說既然有興趣就教你 → 玩家得到紫薇心經 100 點經驗（等於學會）。
條件不符時回話不提等級數字；每人只傳一次。
說話用 say、動作用 emote，一句一行不斷行；有情境停頓才停 1 tick（2 秒）。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define SKILL_ID   "chivi sutra"
#define TAUGHT     "chivi_sutra/taught"
#define TEACHING   "chivi_sutra/teaching"
#define MIN_LEVEL  15
#define TICK       2

void create()
{
    seteuid(getuid());
    set_name("天風道長", ({ "Tianfong taoist", "tianfong", "taoist", "天風道長" }));
    set_race("malik");
    set_class("taoist");
    set_level(40);
    set("gender", "male");
    set("age", 180);
    set("long",
        "天風道長是白雲觀的觀主，原本出身天師道，精研火、風、冰、雷\n"
        "四種破敵咒法。他不願循著前人的路子打磨攻伐之術，反倒另闢蹊\n"
        "徑，將天師四破逐一拆解，推演每一種法術的來龍去脈，進而悟出\n"
        "抵禦元素傷害的防禦法門，這便是流傳於道士與方士之間的「紫薇\n"
        "心經」。如今他避世而不隱世，在觀中修道，也指點有緣人。\n");
    set("custom_faction", "taoist.storm");   // 玄衣派（docs/martial_arts/門派武功對照表.md）
    set_skill(SKILL_ID, 180);
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
    carry_object("/custom/armor/cloth/cyan_taoist_robe")->wear();    // 青色道袍
    carry_object("/custom/armor/head/copper_hat")->wear();           // 青銅道冠
}

int accept_info(object ob, string type)
{
    return type == "skills";
}

private int still_here(object who)
{
    return objectp(who) && environment(who) == environment();
}

// 問到或說到紫薇心經。
private int is_topic(string s)
{
    if( !stringp(s) ) return 0;
    foreach(string p in ({ "，", ",", "﹐", "。", "！", "!", "﹗", "、", " ", "　", "？", "?", "【", "】" }))
        s = replace_string(s, p, "");
    s = lower_case(s);
    return strsrch(s, "紫薇心經") != -1 || strsrch(s, "紫微心經") != -1 || strsrch(s, "chivisutra") != -1;
}

private void begin(object who)
{
    if( who->query(TAUGHT) ) {
        command("say 這門心法我早已傳給你了，往後怎麼修，全看你自己的造化。");
        return;
    }
    if( who->query_temp(TEACHING) ) return;
    if( who->query_class() != "taoist" && who->query_class() != "alchemist" ) {
        command("emote 瞥了" + who->name() + "一眼，輕輕搖了搖頭。");
        command("say 這門心法與你無緣，強求不得。");
        return;
    }
    if( who->query_level() < MIN_LEVEL ) {
        command("say 你的根基尚淺，這門心法此刻教給你，只怕是揠苗助長。");
        command("say 先去外頭多歷練些時日，再來找老道吧。");
        return;
    }
    who->set_temp(TEACHING, 1);
    command("emote 捋了捋長鬚，目光落在" + who->name() + "身上，微微一笑。");
    call_out("teach1", TICK, who);
}

void teach1(object who)
{
    if( !still_here(who) ) { if( objectp(who) ) who->delete_temp(TEACHING); return; }
    command("say 既然你對紫薇心經有興趣，那老道便教你吧。");
    call_out("teach2", TICK, who);
}

void teach2(object who)
{
    if( !still_here(who) ) { if( objectp(who) ) who->delete_temp(TEACHING); return; }
    command("emote 閉目沉吟片刻，將拆解天師四破的心得與逆運心經的訣竅一一道來，" + who->name() + "在一旁細心領會。");
    call_out("teach3", TICK, who);
}

void teach3(object who)
{
    if( !objectp(who) ) return;
    who->delete_temp(TEACHING);
    if( !still_here(who) || who->query(TAUGHT) ) return;
    who->improve_skill_exact(SKILL_ID, 100);   // 100 點經驗就是學會（chivi_sutra.c 的 query_entry_threshold）
    if( who->query_skill(SKILL_ID, 1) > 0 ) who->set(TAUGHT, 1);
}

// ask taoist about 紫薇心經
void accept_inquiry(object who, string key)
{
    if( is_topic(key) ) begin(who);
}

// say 紫薇心經
void relay_say(object who, string msg)
{
    if( who == this_object() || !userp(who) ) return;
    if( is_topic(msg) ) begin(who);
}
