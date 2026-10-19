/*---
description: 天師道法四破（火術、雷法、冰咒、風符）共用的施咒程式。
             設計見 docs/martial_arts/天師道法_四破重製.md（使用者定案，2026-10-18）。
---*/
#include <ansi.h>

inherit SKILL;

string school() { return ""; }
string type() { return "spell"; }
int valid_enable(string usage) { return usage == "spells"; }

// ------------------------------------------------------------------
// 咒文資料
// ------------------------------------------------------------------

string spell_id(string kind, int rank)
{
    if( kind == "fire" )
        return ({ "magic_fire", "earth_fire", "inferno_fire", "pure_fire" })[rank-1];
    if( kind == "freeze" )
        return ({ "chill_freeze", "ominous_freeze", "shadow_freeze", "polar_freeze" })[rank-1];
    if( kind == "storm" )
        return ({ "quick_storm", "crazy_storm", "killer_storm", "whirl_storm" })[rank-1];
    return ({ "swamp_thunder", "blaster_thunder", "divine_thunder", "hexagram_thunder" })[rank-1];
}

/* 可用英文名（底線或空格皆可）或 1～4 指定。 */
int spell_rank(string kind, string spell)
{
    int rank;

    if( !stringp(spell) ) return 0;
    spell = replace_string(spell, " ", "_");
    for(rank = 1; rank <= 4; rank++)
        if( spell == spell_id(kind, rank) || spell == sprintf("%d", rank) )
            return rank;
    return 0;
}

int spell_level(string kind, int rank)
{
    if( kind == "fire" ) return ({ 0, 25, 45, 70 })[rank-1];
    if( kind == "freeze" ) return ({ 0, 25, 45, 70 })[rank-1];
    if( kind == "storm" ) return ({ 0, 30, 55, 85 })[rank-1];
    return ({ 0, 30, 60, 90 })[rank-1];
}

int spell_cost(string kind, int rank)
{
    if( kind == "fire" ) return ({ 5, 20, 45, 75 })[rank-1];
    if( kind == "freeze" ) return ({ 5, 20, 30, 50 })[rank-1];
    if( kind == "storm" ) return ({ 10, 30, 50, 75 })[rank-1];
    return ({ 5, 50, 75, 90 })[rank-1];
}

int spell_delay(string kind, int rank)
{
    if( kind == "fire" ) return ({ 1, 1, 2, 3 })[rank-1];
    if( kind == "freeze" ) return ({ 1, 2, 2, 2 })[rank-1];
    if( kind == "storm" ) return ({ 1, 1, 1, 3 })[rank-1];
    return ({ 1, 1, 2, 2 })[rank-1];
}

string spell_cname(string kind, int rank)
{
    if( kind == "fire" ) return ({ "咒火", "地火", "大業火", "三昧真火" })[rank-1];
    if( kind == "freeze" ) return ({ "寒冰", "兆冰", "無相冰", "七重玄冰" })[rank-1];
    if( kind == "storm" ) return ({ "疾風", "暴風", "飛戮風", "轉輪大風" })[rank-1];
    return ({ "澤雷", "爆雷", "神光雷", "六卦熒雷" })[rank-1];
}

/* 法術名稱（含括號與顏色）：雷黃、火紅、冰白、風紫。
 * 前三招「名稱」；第四招字間空格，前後也空一格。 */
string spell_title(string kind, int rank)
{
    string c;

    if( kind == "fire" ) c = HIR;
    else if( kind == "freeze" ) c = HIW;
    else if( kind == "storm" ) c = HIM;
    else c = HIY;

    // 高亮色帶有粗體，換回青色前要先 NOR 清掉，否則後面的「之術 ....」會跟著變亮。
    if( rank < 4 ) return "「" + c + spell_cname(kind, rank) + NOR CYN "」";
    if( kind == "fire" ) return HIW "『" HIR " 三 昧 真 火 " HIW "』" NOR CYN;
    if( kind == "freeze" ) return HIW "『 七 重 玄 冰 』" NOR CYN;
    if( kind == "thunder" ) return HIY "『 六 卦 熒 雷 』" NOR CYN;
    return "「" HIM " 轉 輪 大 風 " NOR CYN "」";
}

/* 各咒文的效果敘述（命中前，空行後顯示）。 */
string effect_line(string kind, int rank)
{
    if( kind == "fire" ) return ({
        "$N並指一彈，一點赤紅靈焰自指尖躍出，迎風漲成拳頭大小的火球。",
        "$N掌心按地，地面忽然裂開數道火紋，熾熱的地火順著裂縫直竄$n腳下。",
        "$N雙手結印，周身真氣化作熊熊業火，凝成一輪赤紅火輪懸在半空。",
        "$N張口吐出一縷青、白、赤三色交纏的真火，所過之處連空氣都扭曲起來。",
    })[rank-1];
    if( kind == "freeze" ) return ({
        "$N掌中寒氣凝聚，霎時結成一枚晶瑩的冰錐，破空射出。",
        "$N低聲吟誦，一縷幽藍寒氣自袖中悄然滲出，無聲無息地纏上$n。",
        "$N雙手一合，四周空氣驟然凝結，一層看不見的寒意悄悄封向$n的四肢。",
        "$N雙掌連推七次，七重冰輪由外而內層層疊起，將$n困在中央。",
    })[rank-1];
    if( kind == "storm" ) return ({
        "$N袖中風符一閃，一道銳風貼地疾掠，捲起滿地塵沙撲向$n。",
        "$N雙臂一振，四面八方的風勢驟然聚攏，化作一團狂暴的旋風。",
        "$N咒語聲方落，一陣裂帛般的刺耳聲響夾著有如利刃般的狂風捲起，久久不去。",
        "只聽見「嗡」的一聲巨響，霎時天旋地轉，千百條不知是風是刀的細線割裂了你眼前的景象！",
    })[rank-1];
    return ({
        "$N指向地面，一道低沉的雷光貼著地皮蜿蜒而出，滋滋作響地奔向$n。",
        "$N掌中電光驟然收縮成一顆刺目的雷珠，隨手往前一擲。",
        "$N雙目精光一閃，一道清亮如水的雷芒自天而降，無聲無息地罩向$n。",
        "$N腳踏六爻方位，六道熒熒雷光在半空交織成卦，將$n團團圍住。",
    })[rank-1];
}

/* 各咒文的命中敘述。 */
string hit_line(string kind, int rank)
{
    if( kind == "fire" ) return ({
        "火球正中$n胸前，「轟」的一聲炸開，燒得$n衣衫焦黑、皮肉灼痛！",
        "$n腳下猛地噴出一道火柱，烈焰沿著雙腿捲上身來，燙得$n連聲慘叫！",
        "火輪挾著灼人的熱浪轟然壓下，$n避無可避，全身頓時陷入一片火海！",
        "三色真火纏上$n週身，燒得$n皮焦肉爛，慘呼聲中火勢猶自不熄！",
    })[rank-1];
    if( kind == "freeze" ) return ({
        "冰錐正中$n肩頭，寒意透骨而入，凍得$n牙關格格作響！",
        "幽藍寒氣滲入$n體內，$n只覺心頭一寒，精力與神識同時被抽走了一截！",
        "$n周身凝起一層薄冰，手腳僵硬，一時動彈不得！",
        "七重冰輪同時合攏，寒氣如刀割入$n骨髓，凍得$n嘴唇發紫、渾身打顫！",
    })[rank-1];
    if( kind == "storm" ) return ({
        "疾風颳過$n身側，風刃在$n身上劃出幾道細長的血痕！",
        "旋風直撞$n，撕開$n周身的護體氣勁，颳得$n站立不穩、連退數步！",
        "$n受到法術波及，結果只聽見$n一聲慘嚎，銳利如刀的勁風已在$n身上砍出一道深可見骨的可怕傷口！",
        "$n受到法術波及，結果只聽見$n一聲慘嚎，銳利如刀的勁風已在$n身上砍出一道深可見骨的可怕傷口！",
    })[rank-1];
    return ({
        "雷光順著$n雙足竄上全身，電得$n四肢發麻、髮梢直豎！",
        "雷珠在$n身前「轟」地炸開，震得$n耳鳴目眩，胸口一陣翻騰！",
        "雷芒直透$n神識，$n眼前一片白茫，心神恍惚，半晌回不過神來！",
        "數道霹靂自天而降緩緩聚集在$n四週，隨著$N雙手一揮，從四面八方一起扑向$n，結果造成非常可怕的嚴重極傷﹗",
    })[rank-1];
}

/* 落空敘述（取代命中敘述）。 */
string miss_line(string kind)
{
    if( kind == "fire" ) return "$n就地一滾，堪堪避開了撲面而來的火勢。";
    if( kind == "freeze" ) return "$n運氣護住週身，將侵體的寒氣逼了出去。";
    if( kind == "storm" ) return "$n伏低身子，任狂風從頭頂呼嘯而過。";
    return "$n身形急閃，雷光擦身而過，轟在空處。";
}

// ------------------------------------------------------------------
// 數值
// ------------------------------------------------------------------

/* 咒文強度 = (慧根² ÷ 10 + 神 ÷ 32 + random(慧根)) × 咒術倍數；
 * 咒術（基本等級）未滿 100 為 100%，100 級起為 (50 + 咒術)%。神為施咒當時的神。 */
int spell_strength(object me, int sen)
{
    int wis, sp, base;

    wis = me->query_attr("wis");
    sp = me->query_skill("spells", 1);
    base = wis * wis / 10 + sen / 32 + (wis > 0 ? random(wis) : 0);
    if( sp >= 100 ) base = base * (50 + sp) / 100;
    return base;
}

/* 咒文技巧 = 咒術有效等級 + 咒文能力。 */
int spell_technique(object me)
{
    return me->query_skill("spells") + me->query_ability("spell");
}

int resistance(object target, string kind)
{
    int n;

    n = target->query_temp("apply/armor_vs_" + kind);
    if( kind == "storm" ) n += target->query_temp("apply/armor_vs_wind");
    if( kind == "freeze" ) n += target->query_temp("apply/armor_vs_ice");
    if( kind == "thunder" ) n += target->query_temp("apply/armor_vs_lightning");
    if( n < 0 ) n = 0;
    return n;
}

/* 命中率 = 50 + (咒文技巧 − 抗性) × 2，限 5～95%（四系相同）。 */
int spell_hits(object me, object target, string kind, int ignore_resistance)
{
    int chance;

    chance = 50 + (spell_technique(me) - (ignore_resistance ? 0 : resistance(target, kind))) * 2;
    if( chance < 5 ) chance = 5;
    if( chance > 95 ) chance = 95;
    return random(100) < chance;
}

// 裝備的元素傷害力（apply/damage_vs_<元素>）：在抗性之前直接加到傷害上。
int elemental_bonus(object me, string kind)
{
    int bonus;

    bonus = me->query_temp("apply/damage_vs_" + kind);
    if( kind == "storm" ) bonus += me->query_temp("apply/damage_vs_wind");
    if( kind == "freeze" ) bonus += me->query_temp("apply/damage_vs_ice");
    if( kind == "thunder" ) bonus += me->query_temp("apply/damage_vs_lightning");
    return bonus;
}

/* 抗性減傷：火、冰、雷每 1 點抗性減 1 點傷害；風每 2 點減 1 點。最低 1。 */
int elemental_damage(object me, object target, string kind, int power,
    int ignore_resistance)
{
    int resist, damage;

    resist = ignore_resistance ? 0 : resistance(target, kind);
    if( kind == "storm" ) resist /= 2;
    damage = power + elemental_bonus(me, kind) - resist;
    if( damage < 1 ) damage = 1;
    return damage;
}

// ------------------------------------------------------------------
// 顯示
// ------------------------------------------------------------------

void say_line(object me, object target, string msg)
{
    message_vision(CYN + msg + NOR "\n", me, target, 1);
}

void blank(object me)
{
    message_vision("\n", me, 0, 1);
}

int alive_here(object me, object target)
{
    return objectp(target) && living(target) && environment(target) == environment(me);
}

void show_status(object target)
{
    if( objectp(target) ) COMBAT_D->report_status(target);
}

/* 打氣：傷害後顯示命中敘述與體力狀態，傳回實際傷害。 */
int strike_kee(object me, object target, string kind, int rank, int damage)
{
    damage = target->receive_damage(damage, me, me);
    say_line(me, target, hit_line(kind, rank));
    show_status(target);
    return damage;
}

/* 領悟新咒文的敘述（使用者定案，2026-10-19）：該系咒術升級、達到下一道咒文的需求時顯示。
 * 用 taoism/known_<系> 記住已提示到第幾道；第一次（舊角色）以升級前的等級為準，
 * 不會把以前就會的咒文再提示一次。 */
string unlock_line(string kind, int rank)
{
    mapping lines = ([
        "fire": ({ "", "你對火術的領悟更深了一層﹐已能引動地底伏火﹐施展「地火」之術。",
                   "你心火漸旺﹐終於參透了「大業火」的咒訣﹐舉手之間便能焚起熊熊烈焰。",
                   "你精、氣、神三者相合﹐體內陡然燃起一點真火﹐從此能祭起『三昧真火』。" }),
        "freeze": ({ "", "你指尖寒意凝聚不散﹐已能以「兆冰」之術抽取敵人的精神。",
                   "你參透冰無定形之理﹐「無相冰」的咒訣在心中豁然貫通。",
                   "你周身寒氣層層疊起﹐竟有七重之多﹐從此能施展『七重玄冰』。" }),
        "storm": ({ "", "你袖底風聲漸烈﹐已能催起「暴風」之術。",
                   "你御風之術更進一步﹐風中隱含刀鋒﹐「飛戮風」已在掌握之中。",
                   "你心隨風轉﹐四方氣流盡歸調度﹐終於能祭起『轉輪大風』。" }),
        "thunder": ({ "", "你掌心隱隱有雷聲滾動﹐已能施展「爆雷」之術。",
                   "你引雷之術更見精純﹐雷光所至能震人心神﹐「神光雷」已然練成。",
                   "你參透六卦變化﹐天雷地火皆可為用﹐從此能祭起『六卦熒雷』。" }),
    ]);
    return lines[kind][rank-1];
}

void skill_advanced(object me, string skill)
{
    string kind, c;
    int lv, known, r;

    kind = school();
    if( !userp(me) || skill != "taoism-" + kind ) return;
    lv = me->query_skill(skill, 1);
    known = me->query("taoism/known_" + kind);
    if( !known ) {
        known = 1;
        for( r = 2; r <= 4; r++ )
            if( spell_level(kind, r) <= lv - 1 ) known = r;
    }
    if( kind == "fire" ) c = HIR;
    else if( kind == "freeze" ) c = HIW;
    else if( kind == "storm" ) c = HIM;
    else c = HIY;
    for( r = known + 1; r <= 4; r++ ) {
        if( spell_level(kind, r) > lv ) break;
        tell_object(me, c + unlock_line(kind, r) + "\n" NOR);
        known = r;
    }
    me->set("taoism/known_" + kind, known);
}

/* 施咒的技能點數（使用者定案，2026-10-19）：
 * 該系咒術（特殊法術）：(random(悟性) × random(慧根) + random(慧根)) × (random(咒文重數) + 1)，
 *   例如三昧真火 × (random(4)+1)、大業火 × (random(3)+1)；不乘停頓回合。
 * 基本咒術：智力公式把智力換成慧根，再乘咒文重數加成，
 *   ((random(慧根)+1) × (random(慧根)+1) + random(慧根)) × (random(咒文重數) + 1)。
 * 兩者各自擲骰；沒命中只給 1/4、至少 1 點（風符多重催咒只要有一重命中就算命中）。 */
void spell_skill_gain(object me, string kind, int rank, int wis, int hit)
{
    int intel, w, exp;

    if( !userp(me) ) return;
    intel = me->query_attr("int");
    w = wis > 0 ? wis : 1;

    exp = ((random(w) + 1) * (random(w) + 1) + random(w)) * (random(rank) + 1);
    if( !hit && (exp /= 4) < 1 ) exp = 1;
    me->improve_skill_exact("spells", exp);

    exp = ((intel > 0 ? random(intel) : 0) * (wis > 0 ? random(wis) : 0)
        + (wis > 0 ? random(wis) : 0)) * (random(rank) + 1);
    if( !hit && (exp /= 4) < 1 ) exp = 1;
    if( exp > 0 ) me->improve_skill_exact("taoism-" + kind, exp);
}

void award_spell_hit(object me, int amount)
{
    int gain;

    gain = 1 + amount/40;
    if( gain > 20 ) gain = 20;
    me->gain_score("combat", gain);
}

// ------------------------------------------------------------------
// 三昧真火：2 回合後追加的第二發
// ------------------------------------------------------------------

void pure_fire_burst(object me, object target, int damage)
{
    if( !objectp(me) || !alive_here(me, target) ) return;
    blank(me);
    say_line(me, target, "$n體內潛伏的三昧真火猛然爆開，燒得$n渾身顫抖，一時動彈不得！");
    target->receive_damage(damage, me, me);
    show_status(target);
    if( objectp(target) ) target->start_busy(2);
}

// ------------------------------------------------------------------
// 施咒
// ------------------------------------------------------------------

varargs int cast_spell(object me, string spell, object target)
{
    string kind;
    object *enemies;
    int rank, skill, cost, delay, sen, wis, D, power, dealt, stages, i, n, ignore, landed;

    kind = school();
    if( kind != "fire" && kind != "freeze"
    && kind != "storm" && kind != "thunder" ) return 0;
    if( me->skill_mapped("spells") != "taoism-" + kind )
        return notify_fail("你沒有使用這一系咒術。\n");
    if( me->is_busy() ) return notify_fail("你現在沒有空施咒。\n");

    rank = spell_rank(kind, spell);
    if( !rank )
        return notify_fail("指令格式﹕cast <咒文名稱> [on <施咒對象>]\n");
    skill = me->query_skill("taoism-" + kind, 1);
    /* 求教後還沒 gain 的 0 級也算學會（使用者指示，2026-10-19），可以施展需求 0 級的第一道咒文。 */
    if( (skill < 1 && me->query_learn("taoism-" + kind) < 1) || skill < spell_level(kind, rank) )
        return notify_fail("你的咒術修為還不足以施展這道咒文。\n");

    if( !objectp(target) ) {
        enemies = me->query_enemy();
        if( sizeof(enemies) ) target = enemies[0];
    }
    if( !objectp(target) || target == me || !living(target)
    || environment(target) != environment(me) )
        return notify_fail("這裡沒有可施咒的對象。\n");

    cost = spell_cost(kind, rank);
    if( me->query_stat("sen") < cost )
        return notify_fail("你的神不足以施咒。\n");

    sen = me->query_stat("sen");
    wis = me->query_attr("wis");
    me->consume_stat("sen", cost);
    me->fight_ob(target);
    target->fight_ob(me);

    // 每次施咒得法術修為 random(慧根/6) + 1（使用者定案，2026-10-16）。
    me->gain_score("magic", (wis >= 6 ? random(wis / 6) : 0) + 1);

    D = spell_strength(me, sen);
    // 增幅（overload）另外製作；這裡只保留讀取：威力 ×1.5，六卦熒雷不消耗。
    if( me->query_temp("taoism/overload") ) {
        D = D * 3 / 2;
        if( !(kind == "thunder" && rank == 4) )
            me->delete_temp("taoism/overload");
    }
    delay = spell_delay(kind, rank);

    // ---------------- 風符：多重催咒 ----------------
    if( kind == "storm" ) {
        stages = 1;
        if( me->query_temp("taoism/next_storm_three") ) {
            stages = 3;
            me->delete_temp("taoism/next_storm_three");
        } else {
            // 擲 random(咒文能力 + 咒術等級)：80 以上二重（風符 80 級起）、
            // 160 以上三重（風符 120 級起）（使用者定案，2026-10-18）。
            n = random(me->query_ability("spell") + me->query_skill("spells", 1) + 1);
            if( skill >= 120 && n >= 160 ) stages = 3;
            else if( skill >= 80 && n >= 80 ) stages = 2;
        }
        message_vision(CYN "$N口中喃喃念著咒文，開始祭起" + spell_title(kind, rank) + "之術 ....\n" NOR,
            me, target, 1);
        for( i = 2; i <= stages; i++ )
            message_vision(CYN "$N催動法力，轉瞬間祭起第" + (i == 2 ? "二" : "三") + "重"
                + spell_title(kind, rank) + "之術 ....\n" NOR, me, target, 1);

        power = D * ({ 25, 33, 75, 125 })[rank-1] / 100;
        for( i = 1; i <= stages; i++ ) {
            if( !alive_here(me, target) ) break;
            blank(me);
            say_line(me, target, effect_line(kind, rank));
            blank(me);
            if( !spell_hits(me, target, kind, 0) ) {
                say_line(me, target, miss_line(kind));
                continue;
            }
            landed = 1;
            n = elemental_damage(me, target, kind, power, rank == 2);
            n = strike_kee(me, target, kind, rank, n);
            dealt += n;
            // 飛戮風：每一重另扣 random(慧根)/10 的形體。
            if( rank == 3 && objectp(target) && wis >= 10 )
                target->consume_stat("HP", random(wis) / 10, me);
        }
        // 疾風：random(慧根) > 25 時蓄風，下一發風符必定三重。
        if( rank == 1 && wis > 0 && random(wis) > 25 ) {
            me->set_temp("taoism/next_storm_three", 1);
            say_line(me, target, "$N順手將餘風收入袖中，衣袂鼓盪不休，彷彿蓄著一股呼之欲出的風勢。");
        }
        if( dealt > 0 ) award_spell_hit(me, dealt);
        spell_skill_gain(me, kind, rank, wis, landed);
        me->start_busy(delay);
        return 1;
    }

    // ---------------- 其他三系：單發 ----------------
    message_vision(CYN "$N口中喃喃念著咒文，開始祭起" + spell_title(kind, rank) + "之術 ....\n" NOR,
        me, target, 1);
    blank(me);
    say_line(me, target, effect_line(kind, rank));
    blank(me);

    // 地火、神光雷、兆冰無視抗性（命中與傷害都不計）。
    ignore = (kind == "fire" && rank == 2) || (kind == "thunder" && rank == 3)
        || (kind == "freeze" && rank == 2);

    landed = spell_hits(me, target, kind, ignore);
    spell_skill_gain(me, kind, rank, wis, landed);
    if( !landed ) {
        say_line(me, target, miss_line(kind));
        // 無相冰：施法失敗自身停頓 1。
        if( kind == "freeze" && rank == 3 ) delay = 1;
    }
    else if( kind == "freeze" && rank == 3 ) {
        // 無相冰：不造成傷害，對手停頓 3，自己不停頓。
        say_line(me, target, hit_line(kind, rank));
        show_status(target);
        target->start_busy(3);
        award_spell_hit(me, D);
        delay = 0;
    }
    else if( kind == "freeze" && rank == 2 ) {
        // 兆冰：精 −D、神 −D×33%；施法者回復一半。
        target->consume_stat("gin", D, me);
        target->consume_stat("sen", D * 33 / 100, me);
        say_line(me, target, hit_line(kind, rank));
        show_status(target);
        me->supplement_stat("gin", D / 2);
        me->supplement_stat("sen", D * 33 / 200);
        say_line(me, target, "被抽走的寒氣回流$N體內，$N精神為之一振。");
        award_spell_hit(me, D);
    }
    else if( kind == "thunder" && rank == 3 ) {
        // 神光雷：神 −D×33%。
        n = D * 33 / 100;
        target->consume_stat("sen", n, me);
        say_line(me, target, hit_line(kind, rank));
        show_status(target);
        award_spell_hit(me, n);
    }
    else {
        if( kind == "fire" ) power = D * ({ 50, 33, 50, 120 })[rank-1] / 100;
        else if( kind == "freeze" ) power = D * ({ 50, 0, 0, 150 })[rank-1] / 100;
        else power = D * ({ 50, 25, 0, 175 })[rank-1] / 100;
        // 大業火：加上施法者當前氣的 25%；爆雷：加上施法者當前神的 1/8。
        if( kind == "fire" && rank == 3 ) power += me->query_stat("kee") / 4;
        if( kind == "thunder" && rank == 2 ) power += sen / 8;

        n = elemental_damage(me, target, kind, power, ignore);
        dealt = strike_kee(me, target, kind, rank, n);
        award_spell_hit(me, dealt);

        if( kind == "fire" && rank == 2 && dealt > 0 ) {
            me->supplement_stat("kee", dealt / 2);
            say_line(me, target, "地火餘溫回流，$N只覺一股暖意流遍四肢百骸，氣色好了許多。");
        }
        if( kind == "fire" && rank == 4 && wis > 0 && random(wis) > 20 && alive_here(me, target) ) {
            say_line(me, target, "真火餘燼仍在$n體內悶燒，隱隱有再度爆發之勢……");
            call_out("pure_fire_burst", 4, me, target,
                elemental_damage(me, target, kind, D * 180 / 100, 0));
        }
        // 七重玄冰：精、氣、神各自吸取 傷害 × (慧根 − random(慧根))%。
        if( kind == "freeze" && rank == 4 && dealt > 0 && objectp(target) && wis > 0 ) {
            int got;
            foreach(string st in ({ "gin", "kee", "sen" })) {
                n = dealt * (wis - random(wis)) / 100;
                if( n < 1 ) continue;
                target->consume_stat(st, n, me);
                me->supplement_stat(st, n);
                got = 1;
            }
            if( got )
                say_line(me, target, "玄冰寒氣順勢倒捲，將$n的精氣神吸入$N體內。");
        }
    }

    // 雷法反噬：10% 多停頓 1 回合。
    if( kind == "thunder" && random(100) < 10 ) {
        delay++;
        say_line(me, target, "雷力反噬，$N只覺經脈一陣酸麻，需要多花些時間平復氣息。");
    }
    if( delay > 0 ) me->start_busy(delay);
    return 1;
}
