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
        command("say 你都有師父啦，貧道可不好搶人，哈哈，改日來喝茶倒是無妨。");
        return 0;
    }
    if( me->query_attr("wis", 1) < 16 ) {
        command("say 唔，你這孩子跟雷法的緣分還淺了些，不急不急，山長雲多，慢慢走，說不定哪天機緣就到了。");
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
    say_to(who, "雷法這東西，說穿了不過是借天地一口氣，你別怕它兇，也別把它當寶貝，順著性子慢慢來就是。");
    call_out("say_to", TICK, who, "往後想學什麼，開口問便是，貧道閒著也是閒著。");
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "紫衣派弟子");
        me->set("custom_faction", FACTION);
        command("emote 仰頭哈哈一笑，捋了捋花白的鬍鬚");
        command("say 難得今日有緣，好，這個徒弟貧道收下了，從今往後你便是紫衣派的人。");
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
        // 九龍神功的技能檔還沒有製作；做好之前不傳授（基本內功不受影響）。
        if( skill == "nine-dragon force" && file_size("/daemon/skill/nine-dragon_force.c") < 0 ) return 0;
        if( me->query_level() < 10 )
            return notify_fail("華子儀搖頭笑道：這門內功的真氣霸道得很，你的筋骨還嫩，過些時日再來吧。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("華子儀笑道：入門的法子都教給你啦，往後可得靠你自己琢磨。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "華子儀隨手比劃了幾下，將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
