/*---
description: 冰蠶奇功（NEW 新版武功寫法）。素衣派內功，enable 在 force 上使用。
             exert polar-silkworm force／heal／regenerate／vapour／ice-shield。
             設計見 docs/martial_arts/素衣派_冰蠶奇功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define SK_ID  "polar-silkworm force"

private int vapour_serial;   // 每次施展 vapour 的編號，舊的計時器不會誤結束新的效果

private void create()
{
    seteuid(getuid());

    art_id    = SK_ID;
    art_name  = "冰蠶奇功";
    art_usage = "force";
    art_desc  = "素衣派的內功心法，引天山冰蠶的寒氣入體，"
                + "能回復精氣神，大成之後更能化水為霧、借冰幕脫身。";

    art_entry = 20;

    /* 每級武術造詣 等級 × 10；51 級起武學之道 (等級 - 50) × 10。 */
    ma_coef  = 10;
    mm_start = 51;
    mm_minus = 50;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon(SK_ID);
    setup();
}

/* 學習條件：素衣派弟子、角色等級 15；向掌門白齊學習（使用者定案，2026-11-09，取代先前的解謎）。 */
int valid_learn(object me)
{
    if( !is_faction(me, "taoist.freeze") )
        return notify_fail("你不是素衣派弟子﹐無法修練冰蠶奇功。\n");
    if( me->query_level() < 15 )
        return notify_fail("你的根基尚淺﹐還承受不住冰蠶奇功的寒勁。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 20 級門檻，gain 時直接練成 20 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "一縷寒意自丹田升起﹐沿著經脈緩緩流轉﹐你的冰蠶奇功總算有了小成。\n" NOR);
}

/* 每級獎勵（引擎）；練成之後每升一級精、氣、神上限 +1／+1／+2。
 * 80 級：慧根 +1、靈性 +2（永久，只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill(SK_ID, 1);
    if( lv > 20 ) {
        me->advance_stat("gin", 1);
        me->advance_stat("kee", 1);
        me->advance_stat("sen", 2);
    }
    if( lv >= 80 && !me->query("polar_silkworm/bonus_80") ) {
        me->set("polar_silkworm/bonus_80", 1);
        me->set_attr("wis", me->query_attr("wis", 1) + 1);
        me->set_attr("spi", me->query_attr("spi", 1) + 2);
        tell_object(me, HIY "靜坐之間你忽覺靈台一片清冷澄澈﹐冰蠶奇功又上了一層。\n" NOR);
    }
}

/* 修習：每回合消耗 精 1～2、氣 1～2、神 1～3（受傷減少上限 精 1、氣 1、神 3），
 * 得到 基本公式 + random(慧根) 的冰蠶奇功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("gin") < 3 || me->query_stat("kee") < 3 || me->query_stat("sen") < 5 ) {
        tell_object(me, "你只覺寒氣在體內亂竄﹐再也凝聚不起冰蠶真氣。\n");
        return 0;   // 傳回 0 結束打坐
    }

    me->consume_stat("gin", 1 + random(2));
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 1 + random(2));
    me->damage_stat("kee", 1);
    me->consume_stat("sen", 1 + random(3));
    me->damage_stat("sen", 3);

    me->improve_restored_force_tick();
    me->improve_skill_exact(SK_ID,
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("wis")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* heal：消耗精目前值 等級/5，回復氣目前值 等級/2；自身停頓 2 回合。 */
int do_heal(object me)
{
    int sk, cost;

    sk = me->query_skill(SK_ID, 1);
    cost = sk / 5;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你精神耗損過甚﹐運不起冰蠶真氣。\n");

    me->consume_stat("gin", cost);
    me->supplement_stat("kee", sk / 2);
    message_vision("$N吸入一口寒氣﹐化作清涼之意流遍四肢﹐胸口的鬱悶漸漸散去。\n", me);
    me->start_busy(2);
    return 1;
}

/* regenerate：消耗精目前值 5 + 等級/10，回復神目前值 5 + 等級/3；自身停頓 1 回合。 */
int do_regenerate(object me)
{
    int sk, cost;

    sk = me->query_skill(SK_ID, 1);
    cost = 5 + sk / 10;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你精神耗損過甚﹐運不起冰蠶真氣。\n");

    me->consume_stat("gin", cost);
    me->supplement_stat("sen", 5 + sk / 3);
    message_vision("$N閉目凝神﹐一縷冰涼的氣息直透眉心﹐紛亂的心神漸漸澄澈。\n", me);
    me->start_busy(1);
    return 1;
}

/* vapour 加成的四種元素（apply/damage_vs_<元素>，同針術的元氣寫法）。 */
private mapping vapour_bonus(int sk)
{
    mapping bonus = ([]);

    foreach(string e in ({ "fire", "ice", "lightning", "wind" }))
        bonus["damage_vs_" + e] = sk / 4;
    return bonus;
}

private void clear_vapour(object me)
{
    mapping bonus;

    bonus = me->query_temp("polar_silkworm/vapour");
    if( !mapp(bonus) ) return;
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, -val);
    me->delete_temp("polar_silkworm/vapour");
    me->delete_temp("polar_silkworm/vapour_id");
}

void remove_vapour(object me, int id)
{
    if( !objectp(me) || me->query_temp("polar_silkworm/vapour_id") != id ) return;
    clear_vapour(me);
    tell_object(me, HIC "你周身繚繞的寒霧漸漸散去。\n" NOR);
}

/* vapour：消耗全部飲水；冰、火、雷、風四種元素傷害力各 + 等級/4；
 * 持續 等級 個 tick（一 tick 兩秒）；效果中不能再施展；自身停頓 1 回合。 */
int do_vapour(object me)
{
    int sk;
    mapping bonus;

    if( me->query_temp("polar_silkworm/vapour") )
        return notify_fail("你周身已有寒霧繚繞。\n");
    if( me->query_stat("water") <= 0 )
        return notify_fail("你口乾舌燥﹐運不起冰蠶真氣。\n");

    sk = me->query_skill(SK_ID, 1);
    me->consume_stat("water", me->query_stat("water"));
    bonus = vapour_bonus(sk);
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, val);
    me->set_temp("polar_silkworm/vapour", bonus);
    me->set_temp("polar_silkworm/vapour_id", ++vapour_serial);
    message_vision(HIC "$N運起冰蠶真氣﹐腹中飲水化作團團白霧繞身而起﹐四周寒意逼人。\n" NOR, me);
    call_out("remove_vapour", sk * 2, me, vapour_serial);
    me->start_busy(1);
    return 1;
}

/* ice-shield：冰蠶奇功 100 級、冰咒 100 級；只能在戰鬥中使用。
 * 消耗全部飲水，沒有飲水才改用掉 vapour 的效果；兩者都沒有不能用。
 * 使出後必定脫離戰鬥並進入潛行狀態（隱藏值同 sneak 指令：1 + random(潛行 × 2)），
 * 精、氣、神各回復：受損的上限 50%、目前值補滿。不停頓、不累積疲勞。
 * 脫離戰鬥做法同蝶影迷蹤步與茅山鬼煙術。 */
int do_ice_shield(object me)
{
    object *enemies;
    int sneak, water;

    enemies = filter(me->query_enemy(), (: objectp($1) :));
    if( !sizeof(enemies) )
        return notify_fail("你並未與人交手﹐用不著藉冰幕脫身。\n");
    if( me->query_skill(SK_ID, 1) < 100 )
        return notify_fail("你的冰蠶奇功尚未大成﹐凝不起冰幕。\n");
    if( me->query_skill("taoism-freeze", 1) < 100 )
        return notify_fail("你的冰咒修為不足﹐凝不起冰幕。\n");

    water = me->query_stat("water");
    if( water > 0 )
        me->consume_stat("water", water);
    else if( me->query_temp("polar_silkworm/vapour") ) {
        clear_vapour(me);   // 舊的計時器靠編號比對失效，不會誤傷別人
    } else
        return notify_fail("你既沒有飲水﹐身旁也沒有寒霧可借﹐凝不起冰幕。\n");

    message_vision(HIW "$N周身寒霧驟起﹐凝成一層晶瑩冰幕﹐轉眼間人影已隱沒其中。\n" NOR, me);

    foreach(string stat in ({ "gin", "kee", "sen" })) {
        me->heal_stat(stat, (me->query_stat_maximum(stat) - me->query_stat_effective(stat)) / 2);
        me->supplement_stat(stat, me->query_stat_effective(stat) - me->query_stat(stat));
    }

    // 雙方都停手（含殺意）；看得穿的對手之後可以重新出手。
    me->remove_all_killer();
    foreach(object ob in enemies)
        if( objectp(ob) ) ob->remove_killer(me);
    sneak = me->query_skill("sneak");
    me->set_temp("pending/hidden", 1 + (sneak > 0 ? random(sneak * 2) : 0));
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "polar-silkworm force":
        message_vision(HIC "$N盤膝坐下﹐五心向天﹐引一縷冰蠶寒氣在經脈中緩緩運轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "heal":
        return do_heal(me);
    case "regenerate":
        return do_regenerate(me);
    case "vapour":
        return do_vapour(me);
    case "ice-shield":
        return do_ice_shield(me);
    default:
        return notify_fail("冰蠶奇功沒有這種功能。\n");
    }
}
