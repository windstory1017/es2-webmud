#include <ansi.h>
// /d/qiaoyin/npc/girl.c — 喬陰縣 NPC：小女孩（依使用者設計表，2026-10-16）。
// 水雲劍法謎題（使用者定案，2026-10-20）：找她比武（fight）並輸給她，就能拜她為師，
// 再向她求教水雲劍法與她會的其他技能。拜她為師不限門派、種族、職業，也不影響加入其他門派。

#include <npc.h>
inherit F_FIGHTER;

#define LOST_FLAG "sayuensword/lost_to_girl"

void create()
{
    seteuid(getuid());
    set_name("小女孩", ({ "girl" }));
    set_race("human");
    set_class("fighter");
    set_level(5);
    set("gender", "female");
    set("age", 11);
    set("long", @LONG
稚氣未脫的小女孩，扛著一把長劍站在路邊，對著來往的過客大聲吆喝。
LONG
    );
    set_skill("sword", 30);
    set_skill("sayuensword", 30);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    map_skill("sword", "sayuensword");
    set_power("B");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
}

// ------------------------------------------------------------------
// 比武：一律接受（她自己在打或對方正在打時照原規則）。
// ------------------------------------------------------------------

int accept_fight(object ob)
{
    if( is_fighting() || ob->is_fighting() ) return ::accept_fight(ob);
    message_vision(CYN + "$N傲然說道﹕要比武﹐行啊。不過你輸了可要拜我為師喔。\n" NOR, this_object());
    return 1;
}

// 玩家比武輸給她時呼叫（std/char.c 的 lose_fight()）。
void win_fight(object ob)
{
    if( !objectp(ob) || !userp(ob) ) return;
    ob->set(LOST_FLAG, 1);
    // 晚一秒再說，讓這句排在系統的「輸掉了這場比試」之後。
    call_out("laugh_at", 1);
}

void laugh_at()
{
    message_vision(CYN + "$N拍手笑道﹕哈哈哈﹐你輸了﹗\n" NOR, this_object());
}

// ------------------------------------------------------------------
// 拜師：輸給她過就收，不看門派、種族、職業；不改職業、門派、稱號。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( !me->query(LOST_FLAG) ) {
        command("say 你又沒輸給我﹐憑什麼拜我為師﹖");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        message_vision(CYN + "$N挺起胸膛﹐得意地說道﹕好﹐從今以後你就是我的徒弟啦﹗\n" NOR, this_object());
        // 接著叮嚀兩句（初出茅廬﹐講的都是陳腔濫調），每句隔 2 tick（4 秒）；不影響求教。
        call_out("advise", 4, 0);
    }
    return 1;
}

string *ADVICE = ({
    "$N搖頭晃腦地說道﹕記住﹐勤有功、戲無益﹐每天都要練劍知道嗎﹗",
    "$N用稚氣未脫的聲音試著低聲說道﹕打不過就快跑﹐小命最重要﹗",
});

void advise(int i)
{
    message_vision(CYN + ADVICE[i] + "\n" NOR, this_object(), 0, 1);   // 整句顯示、不自動換行
    if( i + 1 < sizeof(ADVICE) ) call_out("advise", 4, i + 1);
}

// ------------------------------------------------------------------
// 傳授：她的徒弟才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( !me->is_apprentice_of(this_object()) ) return 0;

    switch( skill ) {
    case "sayuensword":
    case "sword":
    case "dodge":
    case "parry":
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("小女孩說道﹕該教的都教你啦﹐自己好好練吧。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "小女孩將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
