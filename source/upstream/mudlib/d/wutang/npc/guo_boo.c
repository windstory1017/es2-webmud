#include <ansi.h>
// /d/wutang/npc/guo_boo.c — 五堂鎮 NPC：朱衣派道士 郭布（依五堂鎮設計表）。
// 朱衣派師父（使用者定案，2026-10-18）：平民可拜師，拜師後成為「朱衣派弟子」；
// 角色等級 15 起傳授丹陽神功。

#include <npc.h>
inherit F_FIGHTER;

#define FACTION "taoist.fire"

void create()
{
    seteuid(getuid());
    set_name("郭布", ({ "guo boo", "guo", "boo" }));
    set_race("human");
    set_class("taoist");
    set_level(60);
    set("title", "朱衣派道士");
    set("age", 65);
    set("long", @LONG
一個神色剽悍的禿頭老者﹐若不是穿著道袍﹐說不定你會以為他是個武林豪傑。
LONG
    );
    set_skill("taoism-fire", 80);
    set_skill("spells", 120);
    map_skill("spells", "taoism-fire");
    set("chat_chance_combat", 40);
    set("chat_msg_combat", ({
        (: command, "cast 1" :),
        (: command, "cast 2" :),
        (: command, "cast 3" :),
    }));
    set_skill("eight_steps", 120);
    map_skill("dodge", "eight_steps");
    set_skill("sun force", 100);
    set_skill("force", 100);
    map_skill("force", "sun force");
    set("custom_faction", "taoist.fire");   // 門派記號（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/cloth/red_robe")->wear();
    carry_object("/custom/armor/head/red_hat")->wear();
    carry_object("/custom/weapon/sword/sword_of_redrune")->wield();
}

// ------------------------------------------------------------------
// 拜師：平民可拜師，拜師後成為道士、朱衣派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 你已另有師承﹐貧道不便收你。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "朱衣派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "郭布摸了摸光禿禿的腦袋﹐咧嘴一笑：好﹐從今天起你就是我朱衣派的弟子了。\n",
            "郭布雙目微闔﹐掐指推算了半晌﹐才緩緩說道：心火須日日溫養﹐火熄則道散。好生修持﹐莫負了朱衣一脈的香火。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：朱衣派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "taoism-fire":
    case "spells":
    case "eight_steps":
    case "dodge":
    case "sword":
    case "parry":
    case "unarmed":
        break;
    case "sun force":
    case "force":
        if( me->query_level() < 15 )
            return notify_fail("郭布搖頭道：你根基尚淺﹐還承受不住丹陽神功的熾烈真氣。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("郭布說道：入門的要訣已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "郭布將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
