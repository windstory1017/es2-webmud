/*---
description: 狂擊亂鬥（berserk）。軍人的戰術技能，施展方式見 cmds/std/berserk.c。
             這裡處理升級門檻與升級獎勵（使用者定案，2026-10-12）。
---*/
#include <ansi.h>
inherit SKILL;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("berserk");
    setup();
}

/* 每一級的門檻都是一般門檻的 80%，練滿 200 級共 8,000,000 點。 */
int query_threshold_percent() { return 80; }

/* 升級獎勵（每一級只給一次，berserk/growth_level 記錄已給到第幾級）：
 *   90 級：膂力 +1、膽識 +1、精氣上限各 +40
 *   120 級：膂力 +2、膽識 +2、精氣上限各 +80
 *   140 級：根骨 +7（使用者定案，2026-11-02）
 *   160 級：膂力 +4、膽識 +4、精氣上限各 +120
 *   100 級起每 10 級（100、110……200）：精氣上限各 +10，與上面分開計算 */
private void level_reward(object me, int lv)
{
    int attr, stat;

    switch(lv) {
    case 90:  attr = 1; stat = 40;  break;
    case 120: attr = 2; stat = 80;  break;
    case 160: attr = 4; stat = 120; break;
    }
    if( lv >= 100 && lv % 10 == 0 ) stat += 10;

    if( lv == 140 ) {
        me->set_attr("con", me->query_attr("con", 1) + 7);
        tell_object(me, HIY "多年來你在陣前捨身狂攻﹐挨過的刀槍、受過的震盪不知凡幾﹐\n"
            "一身筋骨在這般千錘百鍊之下脫胎換骨﹐根骨變得堅實強健了！\n" NOR);
    }

    if( attr ) {
        me->set_attr("str", me->query_attr("str", 1) + attr);
        me->set_attr("cor", me->query_attr("cor", 1) + attr);
        tell_object(me, HIY "你在戰陣中殺出了一身悍勇﹐膂力與膽識都增長了！\n" NOR);
    }
    if( stat ) {
        me->advance_stat("gin", stat);
        me->advance_stat("kee", stat);
        tell_object(me, HIY "連番衝殺磨練了你的體魄﹐你的精、氣更加充沛了。\n" NOR);
    }
}

void skill_advanced(object me, string skill)
{
    int lv, done;

    if( !userp(me) ) return;
    lv = me->query_skill("berserk", 1);
    /* 這套獎勵上線前就練過的等級不補發：第一次記錄時從這一級開始。 */
    if( !(done = me->query("berserk/growth_level")) ) done = lv - 1;
    if( done >= lv ) return;
    me->set("berserk/growth_level", lv);
    for(done++; done <= lv; done++) level_reward(me, done);
}
