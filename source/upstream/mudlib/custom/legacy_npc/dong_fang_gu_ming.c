// /custom/legacy_npc/dong_fang_gu_ming.c — 舊站 NPC：東方孤冥（Dong-fang gu ming）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。已放入風城春風客棧二樓（d/fengcheng/inn2.c）。
舊站區域：未能判定；房間：—。
玄天派師父（使用者定案，2026-11-14）：平民或書生可直接拜師（不考慮讀書識字）；已入其他書生門派（例如步玄派）的不收；
拜師後成為書生、「玄天派弟子」（門派代碼 scholar.sky）；拜師才可學，玄天劍訣、玄天心法要自行求教（acquire）。
武功見 docs/martial_arts/玄天教_門派設計.md。
尚未製作的裝備（先空著）：素雲頭巾(Cloud-circlet)。
*/
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "scholar.sky"
#define TICK    2   // 1 tick 約 2 秒

void create()
{
    seteuid(getuid());
    set_name("東方孤冥", ({ "dong-fang gu ming", "dong-fang", "ming" }));
    set("title", "玄天教主");
    set_race("human");
    set_class("scholar");
    set_level(65);
    set("gender", "male");
    set("age", 75);
    set("long",
        "喜好習武﹐年輕時遊走江湖結識各派高手﹐雖然有家傳心法護身﹐卻苦於僅靠家\n"
        "傳劍法難以立足江湖。三十四歲那年﹐他巧得半頁陰心太妃﹐在幾位朋友的解說\n"
        "和指點下創出一套怪異的劍訣配合家傳的玄天劍法頗具威力。數年後﹐他聯絡當\n"
        "初幫他解說陰心太妃的朋友們創立玄天教。\n");
    set("custom_faction", FACTION);   // 玄天派（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/waist/white_jade_girth")->wear();   // 流雲清玉帶
    carry_object("/custom/armor/cloth/white_scholar_dress")->wear();   // 白色長衫
    carry_object("/custom/armor/finger/han_jade_ring")->wear();   // 漢玉戒指
    carry_object("/custom/weapon/sword/silver_sword")->wield();   // 洗銀劍
}

// ------------------------------------------------------------------
// 拜師：種族、性別不限；平民或書生都可以直接拜師，不看讀書識字（使用者定案，2026-11-14）。
// 已拜入其他書生門派（例如步玄派，custom_faction 為 scholar.myst）的不收。
// 拜師後成為書生、玄天派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    string cls = me->query_class(), faction = me->query("custom_faction"), title = me->query("title");

    if( cls == "scholar" ) {
        if( (stringp(faction) && faction != "" && faction != FACTION)
        ||  (stringp(title) && strsrch(title, "步玄") != -1) ) {
            command("say 閣下已是別派門下，老夫不敢奪人所好。");
            return 0;
        }
        return 1;
    }
    if( cls != "commoner" ) {
        command("say 閣下已有師承，老夫不敢奪人所好。");
        return 0;
    }
    return 1;
}

void say_to(object who, string msg)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say " + msg);
}

void welcome(object who)
{
    say_to(who, "玄天一脈講究靜以制動，劍意藏於心而後發，你須沉得住氣，不可浮躁。");
    call_out("say_to", TICK, who, "往後若想求教本門的劍訣心法，儘管開口，老夫自會指點。");
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("scholar");
        me->set("title", "玄天派弟子");
        me->set("custom_faction", FACTION);
        command("say 好，從今日起，你便是我玄天派弟子。");
        call_out("welcome", TICK, me);
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：玄天派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "advance-sky sword":
    case "sword":
    case "parry":
    case "dodge":
        break;
    case "sky force":
    case "force":
        if( me->query_level() < 5 )
            return notify_fail("東方孤冥搖頭道：你的根基尚淺，還領會不了本門心法。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("東方孤冥說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "東方孤冥將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
