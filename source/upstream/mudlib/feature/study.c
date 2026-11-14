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

/* 書本上限看累積的點數（使用者定案，2026-11-09）：一本書正常只給到「升到上限那一級
 * 所需的累積點數」，避免先累積一大堆點數、再一口氣 gain 到超過書的上限。
 * 點數夠了之後的處理見 skill_capped()（2026-11-11 修訂）。 */
int skill_cap_points(object me, mapping caps, string skill)
{
    return me->skill_threshold(skill, caps[skill]);
}

/* 這項技能是否已讀到這本書的上限（使用者定案，2026-11-11，修正卡級）：
 * 看等級：等級到了書的上限才算讀滿。
 * 點數已夠升到上限、但等級還沒 gain 上去時，書仍可以讀，每回合只給極少的點數
 * （見 study_content()），讓玩家一面讀一面 gain，一級一級升到上限；
 * 否則 gain 只升「這段期間有拿到新點數的技能」，點數讀滿後就會卡在原本的等級。 */
int skill_capped(object me, mapping caps, string skill)
{
    if( undefinedp(caps[skill]) ) return 0;
    // 不設點數硬上限（使用者定案，2026-11-12）：等級到了書的上限才算讀滿。
    return me->query_skill(skill, 1) >= caps[skill];
}

/* 書的上限要每一項都讀到了才算讀完（使用者定案，2026-11-07）：例如上限讀書 70、古文 70，
 * 讀書 70、古文 20 時還可以讀，只是讀書不再增加，點數只給還沒到上限的技能。 */
int study_capped(object me, mapping caps)
{
    string skill;

    foreach(skill in keys(caps))
        if( !skill_capped(me, caps, skill) ) return 0;
    return 1;
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
    foreach(skill in keys(skills)) {
        if( skill_capped(me, caps, skill) ) continue;   // 已讀到上限的技能不再增加
        if( undefinedp(caps[skill]) ) me->improve_skill_exact(skill, gain);
        // 一般情況：累積點數最多到升上限那一級所需的點數，超過的部分不給。
        else if( me->query_learn(skill) < skill_cap_points(me, caps, skill) )
            me->improve_skill_exact(skill, gain, skill_cap_points(me, caps, skill));
        // 點數已夠、等級還沒 gain 上去：只給 1 點（另有人物等級加成），
        // 讓 gain 有新點數可以升級。點數本來就超過也照樣給，等級到上限就停讀。
        else me->improve_skill_exact(skill, 1);
    }

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