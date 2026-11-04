/*---
description: 瑯夷派內功（NEW 新版武功寫法）。瑯夷派內功，enable 在 force 上使用。
             exert longyeforce（修習）／refresh（回精）／darkpower（夜遊神）。
             設計見 docs/martial_arts/瑯夷派_瑯夷派內功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION   "thief.longye"
#define ENTRY     10        // 練成等級
#define BONUS_LV  100       // 精上限一次 +120～140
#define DARK_LV   120       // 夜遊神

private void create()
{
    seteuid(getuid());

    art_id    = "longyeforce";
    art_name  = "瑯夷派內功";
    art_usage = "force";
    art_desc  = "瑯夷派的內功心法，陰柔綿密，專練潛伏暗襲的功夫；"
                + "火候深時能化身夜遊神，來去無聲，出手更狠。";

    art_entry = ENTRY;

    /* 每級武術造詣 等級 × 10；30 級起武學之道 (等級 − 29) × 10（比照封山派內功）。 */
    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("longyeforce");
    setup();
}

/* 學習條件：瑯夷派弟子、人物等級 5。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是瑯夷派弟子﹐無法修練瑯夷派內功。\n");
    if( me->query_level() < 5 )
        return notify_fail("你的根基尚淺﹐還摸不透瑯夷派內功的門道。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 10 級門檻，gain 時直接練成 10 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIW "你只覺一股陰柔的真氣潛入四肢百骸﹐瑯夷派內功總算有了小成。\n" NOR);
}

/* 每級獎勵（引擎）；練成之後每升一級精、氣、神上限 +3／+1／+1。
 * 100 級：精上限 +120～140（只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("longyeforce", 1);
    if( lv > ENTRY ) {
        me->advance_stat("gin", 3);
        me->advance_stat("kee", 1);
        me->advance_stat("sen", 1);
    }
    if( lv >= BONUS_LV && !me->query("longyeforce/bonus_100") ) {
        me->set("longyeforce/bonus_100", 1);
        me->advance_stat("gin", 120 + random(21));
        tell_object(me, HIW "你忽覺耳目一清﹐黑暗中的風吹草動都逃不過你的感應﹐瑯夷派內功又上了一層。\n" NOR);
    }
}

/* 修習：每回合消耗 精 4、氣 2、神 1（受傷減少上限 精 2、氣 1），
 * 得到 基本公式 + random(機敏) 的瑯夷派內功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("gin") < 5 || me->query_stat("kee") < 3 || me->query_stat("sen") < 2 ) {
        tell_object(me, "你只覺眼皮沉重、氣息散亂﹐再也凝聚不起瑯夷派的真氣。\n");
        return 0;   // 傳回 0 結束打坐
    }

    me->consume_stat("gin", 4);
    me->damage_stat("gin", 2);
    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("sen", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("longyeforce",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("dex")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh：氣（目前值）不到上限 1/4 不能用；
 * 消耗氣 5 + 等級/10、氣上限受傷 2 + 等級/20；
 * 受損的精上限回復 5 + 等級/5、精回復 10 + 等級/2；自身停頓 2 回合。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("longyeforce", 1);
    cost = 5 + sk / 10;
    if( me->query_stat("kee") * 4 < me->query_stat_maximum("kee")
    ||  me->query_stat("kee") <= cost )
        return notify_fail("你氣血虛浮﹐提不起真氣來振作精神。\n");

    me->consume_stat("kee", cost);
    me->damage_stat("kee", 2 + sk / 20);
    me->heal_stat("gin", 5 + sk / 5);
    me->supplement_stat("gin", 10 + sk / 2);
    message_vision(HIB "$N悄悄吐納了幾口氣﹐眼中的倦意一掃而空。\n" NOR, me, 0, 1);
    me->start_busy(2);
    return 1;
}

void remove_darkpower(object me, mapping bonus)
{
    if( !objectp(me) || !me->query_temp("longyeforce/darkpower") ) return;
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, -val);
    me->delete_temp("longyeforce/darkpower");
    message_vision(HIB "$N朦朧的身影漸漸清晰起來﹐那股鬼魅般的氣息也隨之消散。\n" NOR, me, 0, 1);
}

/* darkpower（夜遊神）：120 級起；精上限受傷 10 + 等級/2；效果中不能再用；戰鬥中可用。
 * 潛行、偷竊 +等級/10；背刺、殺手本能 +等級/8；警覺 +等級/2；行動力 +等級/4；
 * 持續 等級/3 個 tick（一回合兩秒）。 */
int do_darkpower(object me)
{
    int sk, cost;
    mapping bonus;

    sk = me->query_skill("longyeforce", 1);
    if( sk < DARK_LV )
        return notify_fail("你的瑯夷派內功火候未到﹐還無法進入夜遊神之境。\n");
    if( me->query_temp("longyeforce/darkpower") )
        return notify_fail("你正處於夜遊神之境。\n");
    cost = 10 + sk / 2;
    if( me->query_stat_effective("gin") <= cost )
        return notify_fail("你精神耗損過甚﹐無法進入夜遊神之境。\n");

    me->damage_stat("gin", cost);
    bonus = ([
        "sneak":      sk / 10,
        "stealing":   sk / 10,
        "backstab":   sk / 8,
        "killerhood": sk / 8,
        "awarness":   sk / 2,
        "move":       sk / 4,
    ]);
    foreach(string key, int val in bonus) me->add_temp("apply/" + key, val);
    me->set_temp("longyeforce/darkpower", 1);
    message_vision(MAG "$N氣息一斂﹐身形漸漸淡入暗影之中﹐只剩一道朦朧的輪廓﹐已然進入"
                   HIW "『夜 遊 神』" NOR MAG "之境。\n" NOR, me, 0, 1);
    call_out("remove_darkpower", (sk / 3) * 2, me, bonus);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "longyeforce":
        message_vision(MAG "$N縮身伏在暗處﹐氣息放得極輕極細﹐幾乎聽不見呼吸。\n" NOR, me, 0, 1);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "darkpower":
        return do_darkpower(me);
    default:
        return notify_fail("瑯夷派內功沒有這種功能。\n");
    }
}
