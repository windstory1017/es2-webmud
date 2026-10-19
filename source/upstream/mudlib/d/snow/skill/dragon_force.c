

#include <ansi.h>

inherit SKILL;

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("dragon force");
}

int valid_enable(string usage)
{
    return usage == "force";
}

// 學成等級：累積點數到 40 級的門檻前一直是 0 級，gain 時直接練成 40 級。
int query_entry_level() { return 40; }

// 門檻為基本門檻的十分之一：練滿 200 級需 100 萬點（一般技能 1000 萬），
// 學成 40 級需 16,000 點。
int query_threshold_percent() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me,
        HIY "你覺得腹中一股暖洋洋的熱氣忽然膨脹，立刻充滿了全身各處，看來你的\n"
            "龍圖心經已經練成了﹗\n" NOR);
    me->gain_score("martial art", 500);
}

void skill_advanced(object me, string sk)
{
    int level;

    level = me->query_skill(sk, 1);
    if( me->query_stat_maximum("gin") < level * 13 )
	me->advance_stat("gin", 5);
    if( me->query_stat_maximum("kee") < level * 13 )
	me->advance_stat("kee", 5);
    if( me->query_stat_maximum("sen") < level * 13 )
	me->advance_stat("sen", 5);

    // 學成之後每升一級（升到第 level 級）的獎勵，沿用原本的公式。
    if( level > 40 ) {
	me->gain_score("martial art", level * 10);
	if( level > 50 )
	    me->gain_score("martial mastery", (level - 41) * 10);
    }
}

int do_exercise(object me)
{
    if( me->query_stat("gin") < 10
    ||    me->query_stat("kee") < 10
    ||    me->query_stat("sen") < 10 ) {
	tell_object(me, "你覺得神困力乏﹐沒有辦法繼續練功了。\n");
	me->interrupt_me(me, "exhausted");
	return 1;
    }
    me->consume_stat("gin", 10);
    me->consume_stat("kee", 10);
    me->consume_stat("sen", 10);

    if( random(me->query_attr("cps") + me->query_attr("con") + me->query_skill("force", 1)) > 20 ) {
	me->damage_stat("gin", 1);
	me->damage_stat("kee", 1);
	me->damage_stat("sen", 1);
	/* Preserve Dragon Force's canonical special-skill gain, but route the
	 * accompanying BASIC force learned through the user-restored formula. */
	// 龍形神功點數（使用者定案，2026-10-19）：悟性公式 + random(靈性)。
	me->improve_skill_exact("dragon force", COMBAT_D->int_formula_exp(me)
	    + random(me->query_attr("spi")));
	me->improve_restored_force_tick();
    }
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

varargs int
exert_function(object me, string func, object target)
{
    mapping gt;

    switch(func) {
    case "dragon force":
	message_vision(HIY "$N盤膝而坐，深深地吸了口氣，開始修習龍圖心經的內功。\n" NOR,
	    me);
	me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
	return 1;
    default:
	return notify_fail("龍圖心經沒有這種功能。\n");
    }
}

