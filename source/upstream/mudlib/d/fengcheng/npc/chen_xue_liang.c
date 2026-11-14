// /d/fengcheng/npc/chen_xue_liang.c — 風城 NPC：劍甲門傳人 陳學亮（D16 交叉路口）。
//
// 教強化之術（enchance，設計見 docs/martial_arts/劍甲門_強化之術.md）：
//   ask chen about 強化之術 → 條件夠了，他說三句話（遇到同好開心、樂意分享所學、教給玩家）後傳授。
//   條件（程式內部）：法術（magic）60、等級 10、兵刃精熟（weapon mastery）經驗 50000。
//   條件不夠時只說江湖口吻的婉拒，不提數字。

#include <ansi.h>
#include <npc.h>

inherit F_VILLAGER;

#define SKILL_ID  "enchance"
#define TICK      2   // 1 tick 約 2 秒

void create()
{
    seteuid(getuid());
    set_name("陳學亮", ({ "chen xue-liang", "chen", "xue-liang" }));
    set("title", "劍甲門傳人");
    set_race("human");
    set_class("alchemist");
    set_level(60);
    set("gender", "male");
    set("age", 35);
    set("long", "劍甲門的傳人，身形精瘦，雙手布滿了打鐵留下的老繭。劍甲門無門戶之見，但凡對鑄兵鍛甲有天份的人，他都肯傾囊相授。\n");
    set_skill("unarmed", 60);
    set_skill("dodge", 60);
    set_skill("parry", 60);
    set_power("A");
    setup();
}

void say_to(object who, string msg)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say " + msg);
}

void accept_inquiry(object who, string key)
{
    if( key != "強化之術" && key != "enchance" ) return;
    if( who->query_skill(SKILL_ID, 1) > 0 ) {
        command("say 強化之術你已經會了，往後多下些苦功便是。");
        return;
    }
    if( who->query_temp("fengcheng/chen_teaching") ) return;   // 正在傳授，不重複
    if( who->query_skill("magic", 1) < 60 ) {
        command("say 強化之術是法術一路，你的法術根基尚淺，還是先多下些功夫吧。");
        return;
    }
    if( who->query_level() < 10 ) {
        command("say 江湖路上你才走了沒幾步，閱歷太淺，過些時日再來吧。");
        return;
    }
    if( who->query_score("weapon mastery") < 50000 ) {
        command("say 你在兵刃上的歷練還不夠深，不明白武器的脾性，這門功夫教了也是白教。");
        return;
    }
    who->set_temp("fengcheng/chen_teaching", 1);
    command("say 難得遇上同好，貧道今日真是開心！");
    call_out("say_to", TICK, who, "劍甲門的鑄兵鍛甲之術向來不藏私，貧道樂意與有緣人分享所學。");
    call_out("say_to", 2 * TICK, who, "強化之術的法門與要訣，貧道這就傳授給你，往後行走江湖，自有用處。");
    call_out("teach", 3 * TICK, who);
}

void teach(object who)
{
    if( !objectp(who) ) return;
    who->delete_temp("fengcheng/chen_teaching");
    if( environment(who) != environment() ) return;
    who->set_skill(SKILL_ID, 1);
    who->set_learn(SKILL_ID, 1);
    tell_object(who, CYN + "陳學亮將「強化之術」的法術和要訣傳授給你。\n" NOR);
}
