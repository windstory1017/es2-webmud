#include <npc.h>
#include <ansi.h>
inherit F_VILLAGER;

// The ten weapon skills 魯熙年 can pass on via "acquire <skill> from lu" --
// the base five hand-to-hand weapon types, each in its single- and
// two-handed form.
private string *taught_skills = ({
    "axe", "sword", "pike", "staff", "blade",
    "twohanded axe", "twohanded sword", "twohanded pike",
    "twohanded staff", "twohanded blade"
});

void create(){
    set_name("魯熙年", ({"lu xinien","lu","xinien","魯熙年"}));
    set_race("human");
    set_class("soldier"); set_level(25);
    set("gender","male");
    set("title","徵兵官");
    set("long", "魯熙年是一個身材微胖的中年武官﹐兩眼瞇成一條線﹐臉上總是露出似笑非笑的古怪表情。魯熙年同時也是振武營負責招募新兵的募兵官，如果你想從軍，請向他，辦理入伍(enlist)手續。\n");
    set_power("B");
    setup();
    foreach(string sk in taught_skills)
        set_skill(sk, 70);
    // 裝備（使用者指定，2026-10-19）
    carry_object("/custom/weapon/blade/steelblade")->wield();
    carry_object("/custom/armor/head/iron_helmet")->wear();
    carry_object("/custom/armor/waist/iron_girth")->wear();
    carry_object("/custom/armor/armor/ring_armor")->wear();
}

int accept_info(object ob, string type)
{
    return type=="skills";
}

int acquire_skill(object who, string skill)
{
    if( member_array(skill, taught_skills) == -1 ) return 0;

    command("say 好，這門兵刃底子我教你！");
    who->set_skill(skill, 1);
    who->set_learn(skill, 1);
    tell_object(who, CYN + "魯熙年花了好一番功夫，把「" + to_chinese(skill) + "」的基本招式教給了你。\n" NOR);
    return 1;
}

// Called by recruit_stone.c right after a recruit's enlistment succeeds.
// The welcome plays out as a short paced dialogue (1 tick == 2 seconds)
// rather than dumping every line at once; each step re-checks the recruit
// is still here before continuing.
private int still_here(object who)
{
    return who && environment(who)==environment(this_object());
}

void welcome_recruit(object who)
{
    if( !still_here(who) ) return;
    command("say 好小子，敢在徵兵石上按手印，算你有種！");
    call_out("welcome_recruit_2", 2, who);
}

void welcome_recruit_2(object who)
{
    if( !still_here(who) ) return;
    command("say 從今天起，你就是振武軍營的人了！");
    call_out("welcome_recruit_3", 2, who);
}

void welcome_recruit_3(object who)
{
    if( !still_here(who) ) return;
    command("say 戰場上光有一股蠻勁可不夠，我教你幾手實用的搏殺功夫傍身！");
    call_out("welcome_recruit_4", 4, who);
}

void welcome_recruit_4(object who)
{
    if( !still_here(who) ) return;
    command("say 兵刃方面的粗淺入門，隨時可以來找我。");
    call_out("welcome_recruit_5", 2, who);
}

void welcome_recruit_5(object who)
{
    if( !still_here(who) ) return;
    tell_object(who, HIY "你在徵兵石上按下手印，正式成為振武軍營的一名軍人！\n" NOR);
}
