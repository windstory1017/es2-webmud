/* Shared book study rules. */
#include <dbase.h>
#include <name.h>

int study_content(object me);
int halt_study(object me, object from, string how);

int study_cost(object me)
{
    mapping req, caps;
    int wisdom, divisor, cost;

    req = query("required/skill");
    caps = query("content");
    wisdom = me->query_attr("wis");
    divisor = wisdom / 7;
    if( divisor < 1 ) divisor = 1;
    cost = 10;
    if( (mapp(req) && !undefinedp(req["archaic attainment"]))
    || (mapp(caps) && !undefinedp(caps["archaic attainment"])) )
        cost = 12;
    return cost - (1 + random(divisor));
}

/* 書本上限只看技能等級（使用者定案，2026-10-23）：累積點數超過上限等級的門檻
 * 但還沒 gain 升上去時照樣可以讀，讀書拿到新點數才能下 gain 升級。 */
int study_capped(object me, mapping caps)
{
    string skill;
    int limit;

    foreach(skill, limit in caps)
        if( me->query_skill(skill, 1) >= limit ) return 1;
    return 0;
}

void close_study(object me)
{
    message_vision("$N深吸口氣，闔上書本。\n", me);
}

int study_ob(object me)
{
    mapping req, caps;
    string skill, needed_class;
    int minimum;

    if( me->is_busy() ) return notify_fail("你正忙著其他事呢！\n");
    if( me->is_fighting() ) return notify_fail("你還是先應付眼前的敵人吧！\n");

    req = query("required/attribute");
    if( mapp(req) )
        foreach(skill, minimum in req)
            if( me->query_attr(skill, 1) < minimum ) {
                tell_object(me, "你的" + to_chinese(skill) + "不足以閱讀這本書。\n");
                return 1;
            }

    req = query("required/skill");
    if( mapp(req) )
        foreach(skill, minimum in req)
            if( me->query_skill(skill, 1) < minimum ) {
                tell_object(me, "你的" + to_chinese(skill) + "不足以閱讀這本書。\n");
                return 1;
            }

    needed_class = query("required/class");
    if( stringp(needed_class) && me->query_class() != needed_class ) {
        tell_object(me, "你的職業無法研讀這本書。\n");
        return 1;
    }

    caps = query("content");
    if( !mapp(caps) || !sizeof(caps) )
        return notify_fail("這本書沒有可研讀的內容。\n");
    if( study_capped(me, caps) ) {
        tell_object(me, "這本書的內容已經對你了無新意...\n");
        return 1;
    }
    if( me->query_stat("sen") < study_cost(me) ) {
        tell_object(me, "你的神不足以繼續閱讀。\n");
        return 1;
    }

    me->start_busy((: study_content :), (: halt_study :));
    message_vision("$N坐下來開始靜心閱讀" + name() + "。\n", me);
    return 1;
}

int study_content(object me)
{
    mapping req, caps, skills;
    string skill;
    int cost, intellect, gain;

    caps = query("content");
    if( !mapp(caps) || study_capped(me, caps) ) {
        close_study(me);
        return 0;
    }

    cost = study_cost(me);
    if( me->query_stat("sen") < cost ) {
        close_study(me);
        return 0;
    }
    me->consume_stat("sen", cost);

    skills = copy(caps);
    req = query("required/skill");
    if( mapp(req) )
        foreach(skill, int minimum in req) skills[skill] = 1;

    /* 讀書點數（使用者定案，2026-10-19）：和跟王懷芝坐下讀書相同，悟性 × random(悟性)；耗神不變。 */
    intellect = me->query_attr("int");
    gain = intellect * random(intellect);
    foreach(skill in keys(skills))
        me->improve_skill_exact(skill, gain);

    if( study_capped(me, caps) ) {
        close_study(me);
        return 0;
    }
    return 1;
}

int halt_study(object me, object from, string how)
{
    close_study(me);
    return 1;
}