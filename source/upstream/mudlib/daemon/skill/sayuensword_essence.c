/*---
description: 水雲劍法精義。柳東蘆整理過的水雲劍法要訣；不能 enable，
             學會後水雲劍法改用精義版招式並解除等級上限。設計見 docs/martial_arts/共通_水雲劍法.md。
---*/
inherit SKILL;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("sayuensword essence");
    setup();
}

/* 不能 enable 在任何技能上。 */
int valid_enable(string usage) { return 0; }

/* 每級武術造詣 等級 × 10；31 級起武學之道 (等級 − 30) × 10（使用者定案，2026-10-20）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill(skill, 1);
    me->gain_score("martial art", lv * 10);
    if( lv >= 31 ) me->gain_score("martial mastery", (lv - 30) * 10);
}
