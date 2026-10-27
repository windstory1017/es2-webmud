#include <ansi.h>
inherit SKILL;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("tiger force");
    setup();
}

int valid_enable(string usage)
{
    return usage == "force";
}

/*
 * 瘋虎功成長（使用者定案，2026-10-08）
 *
 * 學成：累積 10000 點，gain 時練成 15 級；根骨 +1，並給 15 級本身的每級獎勵。
 * 每級獎勵（15 級起，每升一級）：
 *   武術造詣 + 等級 × 10；31 級起武學之道 + (等級 − 30) × 10
 *   精、氣上限：15～140 級各 +2；141～160 級精 +3、氣 +4；161 級起各 +1。
 *   精（或氣）上限已超過「瘋虎功等級 × 12」時，該項這一級不加（各自判斷，用升級後的等級）。
 * 90 級分歧（只有一次機會，無法回頭）：從 90 升到 91 的那次，
 *   實戰經驗超過 10 萬 → 直接跳到 100 級，膂力 +2、根骨 +1；跳過的 92～99 級不給獎勵。
 *   不到 10 萬 → 照常 91 級，之後一級一級練，100 級沒有額外獎勵。
 * 140 級：膽識 +2。
 * growth_level 記錄已給過獎勵的等級，避免重複發放。
 */
private void level_reward(object me, int lv)
{

    if( !userp(me) ) return;
    if( me->query("tiger_force/growth_level") >= lv ) return;
    me->set("tiger_force/growth_level", lv);

    me->gain_score("martial art", lv * 10);
    if( lv > 30 ) me->gain_score("martial mastery", (lv - 30) * 10);

    // 精氣上限不再受「瘋虎功等級 × 12」限制（使用者定案，2026-10-23）。
    me->advance_stat("gin", lv >= 161 ? 1 : 2 + (lv >= 141 && lv <= 160 ? 1 : 0));
    me->advance_stat("kee", lv >= 161 ? 1 : 2 + (lv >= 141 && lv <= 160 ? 2 : 0));

    if( lv == 140 && !me->query("tiger_force/cor_bonus_140") ) {
        me->set_attr("cor", me->query_attr("cor", 1) + 2);
        me->set("tiger_force/cor_bonus_140", 1);
    }
}

void skill_advanced(object me, string sk)
{
    int lv;

    lv = me->query_skill("tiger force", 1);

    // 90 級分歧：升到 91 的那一刻判斷，只有一次機會。
    if( lv == 91 && userp(me) && !me->query("tiger_force/branch_90") ) {
        me->set("tiger_force/branch_90", 1);
        if( me->query("score/combat") > 100000 ) {
            me->set("tiger_force/jump_100", 1);
            // 跳過的 92～99 級不給獎勵；91 級也算在跳級之內。
            me->set("tiger_force/growth_level", 99);
            me->set_attr("str", me->query_attr("str", 1) + 2);
            me->set_attr("con", me->query_attr("con", 1) + 1);
            tell_object(me, HIR "你身經百戰﹐瘋虎功在生死搏殺間豁然貫通﹐一舉衝破了重重關隘﹗\n"
                "你的膂力大增﹐根骨也更加堅實了。\n" NOR);
            me->advance_skill("tiger force", 9);   // 91 -> 100，會再呼叫一次本函式
            return;
        }
    }

    level_reward(me, lv);
}

// 學成等級：累積 10000 點時 gain，直接練成 15 級。
int query_entry_level() { return 15; }
int query_entry_threshold() { return 10000; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你依法運轉數周天後豁然貫通，終於練成了瘋虎功！根骨也隨之增長一點。\n" NOR);
    if( !me->query("tiger_force/initial_con_bonus") ) {
        me->set_attr("con", me->query_attr("con", 1) + 1);
        me->set("tiger_force/initial_con_bonus", 1);
    }
}

/*
 * User-provided exercise notation: each Tiger Force training tick spends
 * kee 2 / effective kee 1 and gin 3 / effective gin 1.
 * This keeps training behaviour explicit without inventing sen costs.
 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 4 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼，已無法再運轉瘋虎功。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句；2026-10-23）
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 3);
    me->damage_stat("gin", 1);

    /* 每次運功同時累積基本內功與瘋虎功的點數；瘋虎功只能靠修習取得，
     * 戰鬥中不會增加。 */
    me->improve_restored_force_tick();
    me->improve_skill_exact("tiger force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

void remove_powerup(object me, int damage_bonus, int attack_bonus)
{
    if( !objectp(me) || !me->query_temp("tiger_force/powerup") ) return;

    me->add_temp("apply/damage", -damage_bonus);
    me->add_temp("apply/attack", -attack_bonus);
    me->delete_temp("tiger_force/powerup");
    tell_object(me, HIY "你體內奔騰的瘋虎勁逐漸平復，攻勢也恢復如常。\n" NOR);
}

int do_powerup(object me)
{
    int sk, damage_bonus, attack_bonus, duration;

    sk = me->query_skill("tiger force", 1);
    if( sk < 100 )
        return notify_fail("你的瘋虎功火候未足，還無法催動瘋虎強盛勢。\n");
    if( me->query_temp("tiger_force/powerup") )
        return notify_fail("你現在正處於瘋虎強盛勢的催勁狀態。\n");

    damage_bonus = sk / 4;
    attack_bonus = sk / 3;
    duration = sk * 3 / 2;

    me->add_temp("apply/damage", damage_bonus);
    me->add_temp("apply/attack", attack_bonus);
    me->set_temp("tiger_force/powerup", 1);

    message_vision(HIR "$N突然發出幾聲虎吼，雙眼紅絲滿佈，逐漸進入「瘋 虎 強 盛 勢」了！\n" NOR, me);
    call_out("remove_powerup", duration, me, damage_bonus, attack_bonus);
    return 1;
}

/* refresh（瘋虎功練成後才能用）：消耗 瘋虎功/8 的氣（目前值），回復 機敏＋瘋虎功 的精；自身停頓 1 回合，沒有冷卻。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("tiger force", 1);
    if( sk < 1 )
        return notify_fail("你的瘋虎功尚未練成，還無法運氣回精。\n");
    cost = sk / 8;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠，無法運起瘋虎功回復精神。\n");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", me->query_attr("dex") + sk);
    message_vision("$N深吸一口氣，精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "tiger force":
    case "tiger-force":     // 舊寫法，仍可使用
        message_vision(HIY "$N盤膝坐定，調勻呼吸，開始依照瘋虎功口訣運轉內息。\n" NOR,
            me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "powerup":
        return do_powerup(me);
    case "refresh":
        return do_refresh(me);
    default:
        return notify_fail("瘋虎功沒有這種功能。\n");
    }
}
