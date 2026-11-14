// /d/fengcheng/npc/hua_zi_yi.c — 風城 NPC：青雲觀主 華子儀（J10 青雲觀）。紫衣派師父。
//
// 收徒（使用者定案，2026-11-14）：只收平民，慧根（wis）至少 16；拜師後成為道士、「紫衣派弟子」（門派記號 taoist.thunder）。
// 已有師承（其他職業、其他道士門派）的一律不收。拜師才可學：天師道法【雷法】、清河映星劍、陰陽奇門步與基本劍術、
// 招架、閃躲、徒手搏鬥入門即可自行求教（acquire）；九龍神功與基本內功要人物等級 10。

#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.thunder"
#define TICK    2   // 1 tick 約 2 秒

void create()
{
    seteuid(getuid());
    set_name("華子儀", ({ "hua zi-yi", "hua", "zi-yi" }));
    set("title", "青雲觀主");
    set_race("human");
    set_class("taoist");
    set_level(60);
    set("gender", "male");
    set("age", 60);
    set("long", "青雲觀主華子儀，當世少數掌握天師四破雷咒的佼佼者。早年斬妖除魔、保家衛國，如今在青雲觀潛心修行，鬚髮雖已斑白，眼神卻清亮如電。\n");
    set_skill("taoism-thunder", 140);
    set_skill("sword", 100);
    set_skill("dodge", 100);
    set_skill("parry", 100);
    set("custom_faction", FACTION);   // 紫衣派（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/weapon/sword/numinous_sword")->wield();   // 虛靈劍
    carry_object("/custom/armor/head/purple_helmet")->wear();   // 紫雲盔
    carry_object("/custom/armor/cloth/purple_taoist_robe")->wear();   // 紫陽道袍
}

// ------------------------------------------------------------------
// 拜師：平民，慧根至少 16。拜師後成為道士、紫衣派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 閣下已另有師承，貧道不便收你。");
        return 0;
    }
    if( me->query_attr("wis", 1) < 16 ) {
        command("say 貧道看閣下慧根尚淺，與雷法緣分未到，還是另尋他途吧。");
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
    say_to(who, "天師雷法剛猛霸道，卻最怕心浮氣躁，你須先把心性修穩了，雷霆才聽你號令。");
    call_out("say_to", TICK, who, "往後若想求教本門的雷法、劍術與步法，儘管開口，貧道自會指點。");
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "紫衣派弟子");
        me->set("custom_faction", FACTION);
        command("say 好，從今日起，你便是我紫衣派弟子。");
        call_out("welcome", TICK, me);
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：紫衣派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "taoism-thunder":
    case "spells":
    case "reflecting star sword":
    case "sword":
    case "magic steps":
    case "dodge":
    case "parry":
    case "unarmed":
        break;
    case "nine-dragon force":
    case "force":
        if( me->query_level() < 10 )
            return notify_fail("華子儀搖頭道：你的根基尚淺，還承受不住九龍神功的真氣。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("華子儀說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "華子儀將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
