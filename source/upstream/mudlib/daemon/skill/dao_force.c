/*---
description: 明道心法（NEW 新版武功寫法）。玄衣派內功，enable 在 force 上使用。
             exert dao force／recover／remedy／windarmor。設計見 docs/martial_arts/玄衣派_明道心法.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "dao force";
    art_name  = "明道心法";
    art_usage = "force";
    art_desc  = "玄衣派的內功心法，講究清氣上升、濁氣下降，"
                + "能調養經脈、回復精神，大成之後更能引風護體。";

    art_entry = 25;

    /* 每級武術造詣 等級 × 10；30 級起武學之道 (等級 − 29) × 10。 */
    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("dao force");
    setup();
}

/* 學習條件：玄衣派弟子、角色等級 20。 */
int valid_learn(object me)
{
    if( !is_faction(me, "taoist.storm") )
        return notify_fail("你不是玄衣派弟子﹐無法修練明道心法。\n");
    if( me->query_level() < 20 )
        return notify_fail("你的根基尚淺﹐還領會不了明道心法的玄妙。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 25 級門檻，gain 時直接練成 25 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "你氣沉丹田﹐一縷清氣沿著經脈緩緩流轉﹐明道心法總算有了小成。\n" NOR);
}

/* 每級獎勵（引擎）；練成之後每升一級精、氣、神上限 +1／+1／+3。
 * 130 級：慧根 +2，精、氣、神上限各 +30～45（各自擲骰，只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("dao force", 1);
    if( lv > 25 ) {
        me->advance_stat("gin", 1);
        me->advance_stat("kee", 1);
        me->advance_stat("sen", 3);
    }
    if( lv >= 130 && !me->query("dao_force/bonus_130") ) {
        me->set("dao_force/bonus_130", 1);
        me->set_attr("wis", me->query_attr("wis", 1) + 2);
        foreach(string stat in ({ "gin", "kee", "sen" }))
            me->advance_stat(stat, 30 + random(16));
        tell_object(me, HIY "靜坐之間你忽覺心中一片澄明﹐天地間的清氣彷彿隨著呼吸一同起落﹐明道心法又上了一層。\n" NOR);
    }
}

/* 修習：每回合消耗 精 2、氣 2、神 3（受傷減少上限 精 1、氣 1、神 2），
 * 得到 基本公式 + random(慧根) 的明道心法點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("gin") < 3 || me->query_stat("kee") < 3 || me->query_stat("sen") < 4 ) {
        tell_object(me, "你只覺氣息越來越散亂﹐再也凝聚不起明道心法的真氣。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句；2026-10-23）
    }

    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("sen", 3);
    me->damage_stat("sen", 2);

    me->improve_restored_force_tick();
    me->improve_skill_exact("dao force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("wis")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* 等級/(lo) ～ 等級/(hi) 之間擲一個數。 */
private int roll(int sk, int lo_div, int hi_div)
{
    int lo, hi;

    lo = sk / lo_div;
    hi = sk / hi_div;
    return lo + random(hi - lo + 1);
}

/* recover：30 級起；疲勞達 80 就不能用；疲勞 +3，
 * 精、氣、神受損的上限各回復 等級/6～等級/2（各自擲骰）；自身停頓 2 回合。 */
int do_recover(object me)
{
    int sk;

    sk = me->query_skill("dao force", 1);
    if( sk < 30 )
        return notify_fail("你的明道心法火候未到﹐還無法調養受損的經脈。\n");
    if( me->query_stat("fatigue") >= 80 )
        return notify_fail("你實在太累了﹐提不起氣來調息。\n");
    me->supplement_stat("fatigue", 3);
    foreach(string stat in ({ "gin", "kee", "sen" }))
        me->heal_stat(stat, roll(sk, 6, 2));
    message_vision("$N閉上雙眼調息片刻﹐受損的經脈在清氣溫養下漸漸舒緩過來。\n", me, 0, 1);
    me->start_busy(2);
    return 1;
}

/* remedy：60 級起；消耗飲水 50（不足 50 時扣到 0，照樣有效），
 * 精、氣、神目前值各回復 等級/3～等級/2（各自擲骰）；自身停頓 1 回合。 */
int do_remedy(object me)
{
    int sk, water;

    sk = me->query_skill("dao force", 1);
    if( sk < 60 )
        return notify_fail("你的明道心法火候未到﹐還無法以吐納回復精神。\n");
    water = me->query_stat("water");
    me->consume_stat("water", water < 50 ? water : 50);
    foreach(string stat in ({ "gin", "kee", "sen" }))
        me->supplement_stat(stat, roll(sk, 3, 2));
    message_vision("$N緩緩吐納了幾回﹐臉上的倦色漸漸褪去。\n", me, 0, 1);
    me->start_busy(1);
    return 1;
}

void remove_windarmor(object me, mapping bonus)
{
    if( !objectp(me) || !me->query_temp("dao_force/windarmor") ) return;
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, -val);
    me->delete_temp("dao_force/windarmor");
    tell_object(me, HIY "你周身的護體清風漸漸散去。\n" NOR);
}

/* windarmor：明道心法 90 級且風符 90 級；消耗神 等級/2；效果中不能再施展；戰鬥中可用。
 * 守勢等級 +等級/2、護甲 +等級/3、慧根 +(等級/50 + 1)、冰火雷風傷害防禦力各 +等級；
 * 持續 等級 個 tick（一回合兩秒）。 */
int do_windarmor(object me)
{
    int sk, cost;
    mapping bonus;

    sk = me->query_skill("dao force", 1);
    if( sk < 90 )
        return notify_fail("你的明道心法尚未大成﹐還無法引風護體。\n");
    if( me->query_skill("taoism-storm", 1) < 90 )
        return notify_fail("你的風符修為不足﹐引不動護體清風。\n");
    if( me->query_temp("dao_force/windarmor") )
        return notify_fail("你周身已有清風護體。\n");
    cost = sk / 2;
    if( me->query_stat("sen") <= cost )
        return notify_fail("你心神耗損過甚﹐無法引風護體。\n");

    me->consume_stat("sen", cost);
    bonus = ([
        "wittiness":          sk / 2,
        "armor":              sk / 3,
        "wis":                sk / 50 + 1,
        "armor_vs_fire":      sk,
        "armor_vs_ice":       sk,
        "armor_vs_lightning": sk,
        "armor_vs_wind":      sk,
    ]);
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, val);
    me->set_temp("dao_force/windarmor", 1);
    message_vision(HIC "$N捏起劍訣低誦真言﹐周身清風驟起﹐化成一層若隱若現的氣罩護住全身。\n" NOR, me, 0, 1);
    call_out("remove_windarmor", sk * 2, me, bonus);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "dao force":
        message_vision(HIY "$N盤膝坐下﹐依著明道心法的口訣調勻呼吸﹐氣息漸漸變得若有若無。\n" NOR, me, 0, 1);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "recover":
        return do_recover(me);
    case "remedy":
        return do_remedy(me);
    case "windarmor":
        return do_windarmor(me);
    default:
        return notify_fail("明道心法沒有這種功能。\n");
    }
}
