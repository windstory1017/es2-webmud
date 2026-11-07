/*---
description: 新版武功共用引擎（NEW）。武功檔只填資料，選招、出招、傷害加成、
             升級給武術造詣／武學之道都在這裡處理。設計見 docs/martial_arts/。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit SKILL;

/* 以下由武功檔在 create() 裡設定。 */
string art_id;          // 技能代碼，例如 "ksanmando"
string art_name;        // 中文名稱
string art_usage;       // enable 在哪個基本技能，例如 "blade"
string art_desc;        // 武功簡介
int art_bonus;          // 整套傷害加成 (%)
mapping *moves = ({});  // 招式：name action lvl attack defense force bonus weight damage_type

/* 升級獎勵：升到第 lv 級時
 *   武術造詣 = lv × ma_coef
 *   武學之道 = (lv - mm_minus) × mm_coef，lv >= mm_start 才給 */
int ma_coef, mm_start, mm_minus, mm_coef;
string mm_first_msg;    // 第一次獲得武學之道時的訊息（可留空）

/* 學成（配合技能升級的 gain 規則，見 feature/char/skill.c）：
 *   art_entry       學成等級，0 或 1 表示一般從 1 級開始
 *   art_entry_msg   從 0 級學成時的訊息
 *   level_msgs      升到特定等級時的訊息，([ 等級: 訊息 ]) */
int art_entry;
int art_entry_threshold;   // 學成所需累積點數；0 表示照原本算法
int art_entry_reward;      // 1 表示學成那一級也給每級獎勵（預設從下一級起）
string art_entry_msg;
mapping level_msgs = ([]);

int is_martial_art() { return 1; }
string query_art_name() { return art_name; }
string query_base_skill() { return art_usage; }
string query_description() { return art_desc; }
mapping *query_moves() { return moves; }

int valid_enable(string usage) { return usage == art_usage; }

int query_entry_level() { return art_entry > 1 ? art_entry : 1; }
int query_entry_threshold() { return art_entry_threshold; }

/* 學習條件：師父傳授前呼叫 SKILL_D(skill)->valid_learn(me)，傳回 0 表示不能學
 * （武功檔用 notify_fail() 說明原因）。沒有覆寫的武功沒有條件。 */
int valid_learn(object me) { return 1; }

/* 冷梅莊等門派弟子的判定：custom_faction（例如虎刀門是 "fighter.tiger"）。 */
int is_faction(object me, string faction) { return me->query("custom_faction") == faction; }

/* 從 0 級學成時呼叫一次（在升級之前）。 */
void skill_completed(object me, string skill)
{
    if( stringp(art_entry_msg) ) tell_object(me, HIY + art_entry_msg + NOR);
}

/* 目前等級可以使出的招式；一招都不夠格時至少給第一招。 */
mapping *usable_moves(object me)
{
    mapping *list = ({}), m;
    int lv;

    lv = me->query_skill(art_id, 1);
    foreach(m in moves)
        if( m["lvl"] <= lv ) list += ({ m });
    if( !sizeof(list) ) list = moves[0..0];
    return list;
}

/* 依出招權重（預設 5）挑一招。 */
mapping pick_move(object me)
{
    mapping *list, m;
    int total, r, w;

    list = usable_moves(me);
    foreach(m in list) total += m["weight"] ? m["weight"] : 5;
    r = random(total);
    foreach(m in list) {
        w = m["weight"] ? m["weight"] : 5;
        if( r < w ) return m;
        r -= w;
    }
    return list[0];
}

/* 把招式資料轉成 COMBAT_D->fight() 用的 action。
 * pct 是另外乘上的百分比（一般 100）。 */
mapping make_action(mapping m, int pct)
{
    return ([
        "action":      m["action"],
        "attack":      m["attack"],
        "defense":     m["defense"],
        "force":       m["force"],
        "damage_type": m["damage_type"] ? m["damage_type"] : "割傷",
        "damage_pct":  (100 + art_bonus + m["bonus"]) * pct / 100,
    ]);
}

/* 特效用的一擊。flags：HIT_MUST 不能閃躲格擋、HIT_NO_ARMOR 不扣防具、
 * HIT_SILENT 不顯示敘述與體力狀態（由武功自己顯示）。
 * pct 是傷害百分比（以該招平常傷害為準），gin_pct 是耗精百分比（0 不耗精）。
 * 傳回 fight() 的結果。 */
varargs int special_hit(object me, object victim, object weapon, mapping m,
    int pct, int gin_pct, int flags)
{
    mapping act;

    if( !objectp(weapon) ) weapon = 0;
    if( !mapp(m) ) m = moves[random(sizeof(moves))];
    act = make_action(m, pct);
    act["gin_pct"] = gin_pct;
    if( flags & HIT_MUST ) act["must_hit"] = 1;
    if( flags & HIT_NO_ARMOR ) act["no_armor"] = 1;
    if( flags & HIT_SILENT ) act["silent"] = 1;
    return COMBAT_D->fight(me, victim, art_id, act, weapon);
}

/* 目前體力狀態的比例（給 COMBAT_D->status_msg() 用）；不是生物時傳回 -1。 */
int kee_ratio(object ob)
{
    int max;

    if( !objectp(ob) ) return -1;
    max = ob->query_stat_maximum("kee");
    if( !max ) return -1;
    return ob->query_stat("kee") * 100 / max;
}

void show_status(object ob, int ratio)
{
    // 練武假人是死物，不顯示體力狀態。
    if( objectp(ob) && ratio >= 0 && !ob->query("training_dummy") )
        message_vision(COMBAT_D->status_msg(ratio), ob);
}

/* 依技能等級成長的機率：200 級時達到 max（%）。 */
int scaled_chance(object me, int max)
{
    return max * me->query_skill(art_id, 1) / 200;
}

/* 武功檔可以覆寫，處理整套特效。damage 是 fight() 的傳回值。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
}

void attack_using(object me, object opponent, object weapon)
{
    mapping m;
    int damage;

    if( !opponent ) return;
    /* 空手出招時，系統傳來的第三個參數是技能名稱而不是武器，當成沒有武器。 */
    if( !objectp(weapon) ) weapon = 0;
    m = pick_move(me);
    damage = COMBAT_D->fight(me, opponent, art_id, make_action(m, 100), weapon);
    after_strike(me, opponent, weapon, m, damage);
}

/* 每次升級後呼叫；學成時一次跳多級也只呼叫一次。
 * 每級獎勵只給超過學成等級的部分（學成等級為 1 時從 1 級開始給）；
 * art_entry_reward 設為 1 時學成那一級也給。 */
void skill_advanced(object me, string skill)
{
    int lv;
    string msg;

    lv = me->query_skill(art_id, 1);

    if( mapp(level_msgs) && stringp(msg = level_msgs[lv]) )
        tell_object(me, HIY + msg + NOR);

    if( !userp(me) ) return;
    if( art_entry > 1 && (art_entry_reward ? lv < art_entry : lv <= art_entry) ) return;

    if( ma_coef > 0 ) me->gain_score("martial art", lv * ma_coef);

    if( mm_coef > 0 && lv >= mm_start && lv > mm_minus ) {
        me->gain_score("martial mastery", (lv - mm_minus) * mm_coef);
        if( stringp(mm_first_msg) && !me->query("martial_art/" + art_id + "/mastery") ) {
            me->set("martial_art/" + art_id + "/mastery", 1);
            tell_object(me, HIC + mm_first_msg + NOR);
        }
    }
}
