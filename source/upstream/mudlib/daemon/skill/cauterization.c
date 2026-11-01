/*---
description: 針灸之術（cauterization）。武陀灸堂方士的基本技能，四套針法
             （驚異二針、五敗針法、引源針法、三自回針法）enable 在這裡，以 acupuncture 施針。
             每升到 5 的倍數級，精、氣、神上限各 +10。施針時與針法一起累積經驗。
design: docs/martial_arts/武陀灸堂_門派設計.md 第三章（2026-10-27 定案）
---*/
inherit SKILL;

#define SKILL_ID "cauterization"
#define STEP     5
#define BONUS    10

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

int valid_learn(object me)
{
    if( me->query("custom_faction") != "alchemist.wuto" )
        return notify_fail(gender_self(me) + "不是武陀灸堂弟子，學不來針灸之術。\n");
    return 1;
}

// C-06：每升到 5 的倍數級，精、氣、神上限各 +10。已給到哪一級記在
// skill_bonus/cauterization，放棄後重練、重複升到同一級都不會重複給。只對玩家生效。
void skill_advanced(object me, string skill)
{
    int done, now, steps;

    if( !userp(me) ) return;
    now = me->query_skill(SKILL_ID, 1) / STEP * STEP;
    done = me->query("skill_bonus/cauterization");
    if( now <= done ) return;

    me->set("skill_bonus/cauterization", now);
    steps = (now - done) / STEP;
    foreach(string stat in ({ "gin", "kee", "sen" }))
        me->advance_stat(stat, BONUS * steps);
}
