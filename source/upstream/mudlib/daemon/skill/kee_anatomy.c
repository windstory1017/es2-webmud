// 經脈穴道之學（kee anatomy）。依使用者設計表（2026-10-16）；加成規則改版（使用者定案，2026-10-20）。
//
// 每升到 5 的倍數級判定一次：成功時精、氣、神的最大值各加 5；武陀灸堂弟子加 10（使用者定案，2026-10-29）。
// 成功機率 = 等級 × 10 ÷（等級 × 10 + 精氣神最大值中最高的一項），
// 技能越高越容易成功，精氣神越厚越難成功。只對玩家生效。
// 已經判定到哪一級記在 skill_bonus/kee anatomy，同一個門檻一生只判定一次
// （放棄後重練、重複升到同一級都不會重複判定）。提升時不顯示訊息。

inherit SKILL;

#define STEP    5
#define BONUS   5
#define WUTO_BONUS 10   // 武陀灸堂弟子（custom_faction alchemist.wuto）
#define COEF    10

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("kee anatomy");
    setup();
}

void skill_improved(object me, string skill)
{
    me->apply_restored_skill_progression(skill);
}

private int highest_stat(object me)
{
    int top, v;

    foreach(string stat in ({ "gin", "kee", "sen" }))
        if( (v = me->query_stat_maximum(stat)) > top ) top = v;
    return top;
}

void skill_advanced(object me, string skill)
{
    int done, now, lv, bonus;

    if( !userp(me) ) return;

    now = me->query_skill(skill, 1) / STEP * STEP;
    done = me->query("skill_bonus/kee anatomy");
    if( now <= done ) return;

    me->set("skill_bonus/kee anatomy", now);
    bonus = me->query("custom_faction") == "alchemist.wuto" ? WUTO_BONUS : BONUS;
    for(lv = done + STEP; lv <= now; lv += STEP) {
        if( random(lv * COEF + highest_stat(me)) >= lv * COEF ) continue;
        foreach(string stat in ({ "gin", "kee", "sen" }))
            me->set_stat_maximum(stat, me->query_stat_maximum(stat) + bonus);
    }
}
