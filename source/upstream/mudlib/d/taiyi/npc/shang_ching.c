// /d/taiyi/npc/shang_ching.c — 太乙觀：上清道長（Shang-ching，依太乙觀設計表，2026-11-09）。
/*
白齊的得意門徒（使用者更正，2026-11-09），45 歲。素衣派的五行步法由他親自傳授：
素衣派弟子才教；只補足到剛好可以升到 1 級的經驗。設計見 docs/martial_arts/素衣派_五行迷蹤步.md。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.freeze"

void create()
{
    seteuid(getuid());
    set_name("上清道長", ({ "Shang-ching taoist", "shang-ching", "taoist", "上清道長" }));
    set_race("human");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 45);
    set("title", "太乙觀道長");
    set("long", @LONG
一位身著青衣道袍的道長，面容清瘦，目光澄澈，走起路來輕盈得幾乎不沾地面。他是太乙觀觀主白齊的得意門徒，素衣派的五行步法，皆由他親自傳授。
LONG
    );
    set("custom_faction", FACTION);   // 素衣派（docs/martial_arts/門派武功對照表.md）
    set_skill("five-steps", 100);
    map_skill("dodge", "five-steps");
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

int accept_info(object ob, string type)
{
    return type == "skills";
}

// ------------------------------------------------------------------
// 傳授：素衣派弟子才教五行迷蹤步；只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;
    if( skill != "five-steps" ) return 0;

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("上清道長說道：入門的步法已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "上清道長將「" + to_chinese(skill) + "」的入門步法傳授給你。\n" NOR);
    return 1;
}
