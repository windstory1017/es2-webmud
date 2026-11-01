/*---
description: 武陀灸術【三自回針法】（recovery-needle）。武陀灸堂方士的自療針法，enable 在
             針灸之術（cauterization）上，以 acupuncture 施針，只能扎在自己身上：
             gin-up、kee-up、sen-up 回復精、氣、神與受傷減少的上限，每次疲勞 +1。
             學習條件：經脈穴道之學 80。
design: docs/martial_arts/武陀灸堂_門派設計.md 第七章（2026-10-27 定案）
---*/
#include <ansi.h>

inherit SKILL;

#define EXP_TIER 3   // 經驗倍數 1～3（使用者定案，2026-10-28）
#include "/daemon/skill/wuto_needle.h"

#define SKILL_ID "recovery-needle"

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

int valid_learn(object me)
{
    if( !check_wuto(me) ) return 0;
    if( me->query_skill("kee anatomy", 1) < 80 )
        return notify_fail(gender_self(me) + "對經脈穴道的認識還太淺，摸不著三自回針法的門徑。\n");
    return 1;
}

void skill_completed(object me, string skill)
{
    tell_object(me, HIY + gender_self(me) + "終於練成了三自回針法，三針分走精、氣、神三路，"
        "自身氣血已能收放由心。\n" NOR);
}

// 回精針扎百會、回氣針扎氣海、回神針扎神庭，各一句；$W 換成手上的針。
mapping MSG = ([
    "gin": "$N將$W扎進自己頭頂百會穴，一股清氣自頂門灌下，昏沉的腦袋頓時清醒不少。",
    "kee": "$N以$W在臍下氣海穴緩緩捻入一針，丹田一暖，紊亂的氣息漸漸平順下來。",
    "sen": "$N以$W輕點眉心神庭穴，雙目微闔片刻，再睜開時眼神已清明許多。",
]);

/* R-01～R-06：疲勞 +1；回復該項 sk×2/3，受傷減少的上限 sk/3（先回上限再回現值）。 */
varargs int acupuncture_cauterization(object me, string spl, object target)
{
    string stat;
    int sk;

    if( !knows_needle(me, SKILL_ID) )
        return notify_fail(gender_self(me) + "還沒學會三自回針法。\n");
    switch(spl) {
    case "gin-up": case "gin_up": case "回精針": stat = "gin"; break;
    case "kee-up": case "kee_up": case "回氣針": stat = "kee"; break;
    case "sen-up": case "sen_up": case "回神針": stat = "sen"; break;
    default:
        return notify_fail("三自回針法沒有這種針法。\n");
    }
    if( objectp(target) && target != me )
        return notify_fail("三自回針法只能扎在自己身上。\n");
    if( me->query_stat("fatigue") >= me->query_stat_maximum("fatigue") )
        return notify_fail(gender_self(me) + "實在太累了，手指發顫，連針都捏不住。\n");

    sk = me->query_skill(SKILL_ID, 1);
    me->supplement_stat("fatigue", 1);
    me->heal_stat(stat, sk / 3);
    me->supplement_stat(stat, sk * 2 / 3);
    // 整句青色，一句帶過；三自回針法不取招式名。
    needle_line(me, 0, replace_string(MSG[stat], "$W", needle_name(me)));

    needle_gain(me, SKILL_ID);
    me->start_busy(1);
    return 1;
}
