/*---
description: 強力格擋（heavy_parry）。軍人的格擋技巧，由米沛傳授。
             效果（格擋力道上限放大）見 feature/char/combat.c 的 parry_attack()；
             經驗來自招架成功，見 COMBAT_D->restored_parry_gain()。
             這裡處理學成門檻與升級獎勵（使用者定案，2026-10-12）。
---*/
#include <ansi.h>
inherit SKILL;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("heavy_parry");
    setup();
}

/* 學成：累積 5000 點，gain 時直接練成 10 級；之後照一般門檻。 */
int query_entry_level() { return 10; }
int query_entry_threshold() { return 5000; }

/* 定力成長機率（同招架成長屬性）：基礎機率 × (100 + 等級) × 0.9 %，最高 100%，屬性沒有上限（2026-11-12）。 */
private int cps_growth_base(int value)
{
    if( value <= 10 ) return 100;
    if( value <= 15 ) return 90;
    if( value <= 20 ) return 80;
    if( value <= 25 ) return 60;
    if( value <= 30 ) return 40;
    if( value <= 40 ) return 30;
    if( value <= 45 ) return 15;
    return 10;
}

/* 每級獎勵（學成那一級起，不補 1～9 級）：
 *   精上限 +1、氣上限 +1～2、神上限 +0～1
 *   每 10 級擲一次定力成長（同招架的機率）
 *   100、120、140、160、180、200 級定力固定 +1（與上面分開計算） */
private void level_reward(object me, int lv)
{
    int cps, chance;

    me->advance_stat("gin", 1);
    me->advance_stat("kee", 1 + random(2));
    me->advance_stat("sen", random(2));

    if( lv % 10 == 0 ) {
        cps = me->query_attr("cps", 1);
        chance = cps_growth_base(cps) * (100 + lv) * 9 / 10;
        if( chance > 10000 ) chance = 10000;
        if( random(10000) < chance ) {
            me->set_attr("cps", cps + 1);
            tell_object(me, HIY "你的定力提高了！\n" NOR);
        }
    }

    if( lv >= 100 && lv % 20 == 0 ) {
        me->set_attr("cps", me->query_attr("cps", 1) + 1);
        tell_object(me, HIY "你對強力格擋的掌握讓你的定力提升了！\n" NOR);
    }
}

void skill_advanced(object me, string skill)
{
    int lv, done;

    if( !userp(me) ) return;
    lv = me->query_skill("heavy_parry", 1);
    if( lv < 10 ) return;
    /* 這套獎勵上線前就練過的等級不補發；從 0 學成時從 10 級開始給。 */
    if( !(done = me->query("heavy_parry/growth_level")) ) done = lv - 1;
    if( done >= lv ) return;
    me->set("heavy_parry/growth_level", lv);
    for(done++; done <= lv; done++) level_reward(me, done);
}
