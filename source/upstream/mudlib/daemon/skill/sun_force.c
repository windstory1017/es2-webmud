/*---
description: 丹陽神功（NEW 新版武功寫法）。朱衣派內功，enable 在 force 上使用。
             exert sun force／remedy／recover／sun fire。設計見 docs/martial_arts/道士_清河映星劍與奇門步法.md。
---*/
#include <ansi.h>
#include <localtime.h>

inherit "/std/martial_art";

#define TIME_TICK (time()*60%1892160000)   // 與 NATURE_D 相同的遊戲時間

private void create()
{
    seteuid(getuid());

    art_id    = "sun force";
    art_name  = "丹陽神功";
    art_usage = "force";
    art_desc  = "朱衣派的內功心法，引丹田一點純陽之氣運轉周天，"
                + "能回復精氣神、溫養受損的經脈，大成之後更能於白晝採聚純陽之氣，助長咒術。";

    art_entry = 10;

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("sun force");
    setup();
}

/* 學習條件：等級 15、朱衣派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "taoist.fire") )
        return notify_fail("你不是朱衣派弟子﹐無法修練丹陽神功。\n");
    if( me->query_level() < 15 )
        return notify_fail("你的根基尚淺﹐還承受不住丹陽神功的熾烈真氣。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 10 級門檻，gain 時直接練成 10 級；練成時慧根 +2。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "一股暖意自丹田升起﹐如旭日初昇般流遍周身﹐你的丹陽神功已有小成﹐靈台也隨之清明了幾分。\n" NOR);
    if( userp(me) ) me->set_attr("wis", me->query_attr("wis", 1) + 2);
}

/* 每級獎勵（引擎）；超過學成等級後，每級精 +1、氣 +1、神 +3
 * （不再受上限條件限制，使用者定案，2026-10-23）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("sun force", 1);
    if( lv <= 10 ) return;
    me->advance_stat("gin", 1);
    me->advance_stat("kee", 1);
    me->advance_stat("sen", 3);
}

/* 修習：每回合消耗 精 2、氣 2～3、神 3～4（受傷減少上限 精 1、氣 1、神 3），
 * 得到 基本公式 + random(慧根) 的丹陽神功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("gin") < 3 || me->query_stat("kee") < 4 || me->query_stat("sen") < 7 ) {
        tell_object(me, "你只覺口乾舌燥、心火虛浮﹐再也運轉不動丹陽真氣。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句；2026-10-23）
    }

    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 2 + random(2));
    me->damage_stat("kee", 1);
    me->consume_stat("sen", 3 + random(2));
    me->damage_stat("sen", 3);

    me->improve_restored_force_tick();
    me->improve_skill_exact("sun force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("wis")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* remedy：消耗飲水 50，精、氣、神目前值各回復 丹陽神功/2；自身停頓 1 回合。 */
int do_remedy(object me)
{
    int sk;

    if( me->query_stat("water") < 50 )
        return notify_fail("你口乾舌燥﹐運不起丹陽真氣。\n");
    sk = me->query_skill("sun force", 1);
    me->consume_stat("water", 50);
    me->supplement_stat("gin", sk / 2);
    me->supplement_stat("kee", sk / 2);
    me->supplement_stat("sen", sk / 2);
    message_vision("$N深吸一口氣﹐丹田純陽之氣上湧﹐蒼白的臉色漸漸泛起紅潤。\n", me);
    me->start_busy(1);
    return 1;
}

/* recover：丹陽神功 30 級起；疲勞 +1，精、氣、神受損的上限各回復 丹陽神功/4；自身停頓 2 回合。 */
int do_recover(object me)
{
    int sk;

    sk = me->query_skill("sun force", 1);
    if( sk < 30 )
        return notify_fail("你的丹陽神功火候未到﹐還無法以純陽之氣溫養受損的經脈。\n");
    if( me->query_stat("fatigue") >= me->query_stat_maximum("fatigue") )
        return notify_fail("你身心俱疲﹐已無力再運轉丹陽真氣療傷。\n");
    me->supplement_stat("fatigue", 1);
    me->heal_stat("gin", sk / 4);
    me->heal_stat("kee", sk / 4);
    me->heal_stat("sen", sk / 4);
    message_vision("$N雙掌合於胸前﹐一股溫熱的純陽之氣在體內往復流轉﹐受損的經脈漸漸舒展開來。\n", me);
    me->start_busy(2);
    return 1;
}

/* 白天：依 NATURE_D 的天色表，日出到傍晚（太陽還在天上）算白天。
 * 天色表每段的長度從午夜起累加，算法和 NATURE_D 的 init_day_phase() 相同。 */
int is_daytime()
{
    mapping *phases;
    mixed *local;
    int i, t;

    phases = NATURE_D->query_day_phase();
    if( !arrayp(phases) || !sizeof(phases) ) return 1;
    local = localtime(TIME_TICK);
    t = local[LT_HOUR] * 60 + local[LT_MIN];
    i = 0;
    foreach(mapping phase in phases) {
        if( t < phase["length"] ) break;
        t -= (int)phase["length"];
        i++;
    }
    i = (i == 0 ? sizeof(phases) - 1 : i - 1);
    return member_array(phases[i]["event_fun"],
        ({ "event_sunrise", "event_morning", "event_noon", "event_afternoon", "event_evening" })) >= 0;
}

void remove_sun_fire(object me, int wis, int cps, int spell)
{
    if( !objectp(me) || !me->query_temp("sun_force/sun_fire") ) return;
    me->add_temp("apply/wis", -wis);
    me->add_temp("apply/cps", -cps);
    me->add_temp("apply/spell", -spell);
    me->delete_temp("sun_force/sun_fire");
    tell_object(me, HIY "你體內的純陽之氣漸漸散去﹐周身的赤紅光暈也隨之隱沒。\n" NOR);
}

/* 聚日術：丹陽神功 90 級起；只能在白天施展（施展後入夜不會中斷）；消耗神 150；
 * 慧根 +5～8、定力 +5～8、咒文能力 +丹陽神功/2；持續 丹陽神功 回合（一回合兩秒）。 */
int do_sun_fire(object me)
{
    int sk, wis, cps, spell;

    sk = me->query_skill("sun force", 1);
    if( sk < 90 )
        return notify_fail("你的丹陽神功尚未大成﹐聚不起純陽之氣。\n");
    if( me->query_temp("sun_force/sun_fire") )
        return notify_fail("你體內的純陽之氣尚未散去。\n");
    if( !is_daytime() )
        return notify_fail("天上沒有日光﹐你無從採聚純陽之氣。\n");
    if( me->query_stat("sen") <= 150 )
        return notify_fail("你心神耗損過甚﹐無法凝聚純陽之氣。\n");

    me->consume_stat("sen", 150);
    wis = 5 + random(4);
    cps = 5 + random(4);
    spell = sk / 2;
    me->add_temp("apply/wis", wis);
    me->add_temp("apply/cps", cps);
    me->add_temp("apply/spell", spell);
    me->set_temp("sun_force/sun_fire", 1);
    message_vision(HIR "$N仰首向日﹐雙掌虛托﹐點點純陽之氣如金屑般匯入掌心﹐周身隱隱泛起一層赤紅光暈。\n" NOR, me);
    call_out("remove_sun_fire", sk * 2, me, wis, cps, spell);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "sun force":
        message_vision(HIY "$N盤膝坐下﹐五心向天﹐引丹田一點純陽之氣緩緩運轉周天。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "remedy":
        return do_remedy(me);
    case "recover":
        return do_recover(me);
    case "sun fire":
        return do_sun_fire(me);
    default:
        return notify_fail("丹陽神功沒有這種功能。\n");
    }
}
