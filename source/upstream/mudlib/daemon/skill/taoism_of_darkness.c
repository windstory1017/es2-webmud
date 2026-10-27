/*---
description: 茅山奇術（taoism of darkness）。幻霧觀專長咒術，enable 在咒術（spells）上，以 cast 施展；
             每次施展消耗一張少陰符，不另外消耗精、氣、神。
             ghost_fire 鬼火術、smoke 鬼煙術、hell_fire 幽冥之火。
             設計見 docs/martial_arts/幻霧觀_茅山奇術與奇門術.md。
---*/
#include <ansi.h>

inherit SKILL;

#define SCROLL  "/custom/item/scroll/yin"
#define SPELLS_D "/daemon/skill/taoism-spells"
#define FACTION "taoist.fog"

string type() { return "spell"; }
int valid_enable(string usage) { return usage == "spells"; }

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("taoism of darkness");
    setup();
}

/* 學習條件：幻霧觀弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != FACTION )
        return notify_fail("你不是幻霧觀弟子﹐參不透茅山奇術的門道。\n");
    return 1;
}

/* 累積 30000 點學成 20 級（使用者定案，2026-10-24）。 */
int query_entry_level() { return 20; }
int query_entry_threshold() { return 30000; }

void skill_completed(object me, string skill)
{
    tell_object(me, HIK "你對著少陰符低誦咒文﹐指間忽地竄起一點幽綠的火光﹐茅山奇術總算入了門。\n" NOR);
}

// ------------------------------------------------------------------
// 咒文資料（使用者定案，2026-10-24）
// ------------------------------------------------------------------

/* 咒文：可用等級、停頓、咒文重數（算點數用）、名稱。 */
mapping SPELLS = ([
    "ghost_fire": ({  0, 1, 1, "鬼火術" }),
    "smoke":      ({ 30, 1, 2, "鬼煙術" }),
    "hell_fire":  ({ 50, 1, 3, "幽冥之火" }),
]);

/* 施咒點數（照天師咒術）：
 * 基本咒術：((random(慧根)+1) × (random(慧根)+1) + random(慧根)) × (random(重數)+1)
 * 茅山奇術：(random(悟性) × random(慧根) + random(慧根)) × (random(重數)+1)
 * 沒打中只給 1/4、至少 1 點。 */
void spell_gain(object me, int rank, int hit)
{
    int wis, intel, exp;

    if( !userp(me) ) return;
    wis = me->query_attr("wis");
    intel = me->query_attr("int");
    if( wis < 1 ) wis = 1;

    exp = ((random(wis) + 1) * (random(wis) + 1) + random(wis)) * (random(rank) + 1);
    if( !hit && (exp /= 4) < 1 ) exp = 1;
    me->improve_skill_exact("spells", exp);

    exp = ((intel > 0 ? random(intel) : 0) * random(wis) + random(wis)) * (random(rank) + 1);
    if( !hit && (exp /= 4) < 1 ) exp = 1;
    if( exp > 0 ) me->improve_skill_exact("taoism of darkness", exp);
}

object find_scroll(object me)
{
    foreach(object ob in all_inventory(me))
        if( base_name(ob) == SCROLL ) return ob;
    return 0;
}

// ------------------------------------------------------------------
// 顯示：敘述灰色、咒文名稱暗紅色；排版照天師咒術
// （起手一行、空行、效果、空行、結果）。
// ------------------------------------------------------------------

void say_line(object me, object target, string msg)
{
    message_vision(HIK + msg + NOR "\n", me, target, 1);
}

void blank(object me)
{
    message_vision("\n", me, 0, 1);
}

string spell_title(string spell)
{
    return "「" NOR RED + SPELLS[spell][3] + NOR HIK "」";
}

void start_line(object me, object target, string spell)
{
    say_line(me, target, "$N拈起一張少陰符﹐口中念念有詞﹐開始施展" + spell_title(spell) + " ....");
    blank(me);
}

void award_spell_hit(object me, int amount)
{
    int gain;

    gain = 1 + amount / 40;
    if( gain > 20 ) gain = 20;
    me->gain_score("combat", gain);
}

// ------------------------------------------------------------------
// 鬼火術、幽冥之火：命中照天師咒術（不看抗性）；
// 鬼火術 咒文強度 × 33%；幽冥之火 咒文強度 + (random(慧根)+1) × 2。
// ------------------------------------------------------------------

int do_fire(object me, object target, string spell, int sen)
{
    int wis, power, dealt;

    wis = me->query_attr("wis");
    me->fight_ob(target);
    target->fight_ob(me);

    start_line(me, target, spell);
    if( spell == "ghost_fire" )
        say_line(me, target, "$N將少陰符夾在指間一晃﹐符紙化作一團幽綠鬼火﹐飄飄忽忽地撲向$n。");
    else
        say_line(me, target, "$N雙手捧起少陰符﹐符紙忽地化作一團青黑色的冥火﹐挾著陰風捲向$n。");
    blank(me);

    if( !SPELLS_D->spell_hits(me, target, "darkness", 1) ) {
        if( spell == "ghost_fire" )
            say_line(me, target, "$n側身一讓﹐鬼火擦肩飄過﹐在半空中無聲熄滅。");
        else
            say_line(me, target, "$n拚命往旁一滾﹐冥火落在地上﹐燒出一圈焦黑的痕跡。");
        return 0;
    }

    power = SPELLS_D->spell_strength(me, sen);
    if( spell == "ghost_fire" ) power = power * 33 / 100;
    else power += ((wis > 0 ? random(wis) : 0) + 1) * 2;
    if( power < 1 ) power = 1;

    dealt = target->receive_damage(power, me, me);
    if( spell == "ghost_fire" )
        say_line(me, target, "鬼火貼著$n的身子一燎﹐$n只覺一股陰寒直透骨髓。");
    else
        say_line(me, target, "冥火轟然撲上$n的身子﹐燒得$n慘號連連﹐彷彿連魂魄都要焚盡了。");
    if( objectp(target) ) COMBAT_D->report_status(target);
    award_spell_hit(me, dealt);
    return 1;
}

// ------------------------------------------------------------------
// 鬼煙術：留在原地脫離戰鬥並隱藏；隱藏值 1 + random((潛行 + 茅山奇術) ÷ 2 × 2)。
// 看不穿的對手失去你的蹤影；看得穿的只停手一次，之後可以重新出手。
// ------------------------------------------------------------------

int do_smoke(object me)
{
    object *enemies, *seen = ({});
    int eff;

    enemies = filter(me->query_enemy(), (: objectp($1) :));
    if( !sizeof(enemies) )
        return notify_fail("你並未與人交手﹐用不著遁入煙中。\n");

    start_line(me, 0, "smoke");
    say_line(me, 0, "$N將少陰符往腳下一擲﹐地上騰起一股灰黑濃煙﹐霎時將$N的身形吞沒。");
    blank(me);

    eff = (me->query_skill("sneak") + me->query_skill("taoism of darkness")) / 2;
    // 雙方都停手（含殺意）；看得穿的對手之後可以重新出手。
    me->remove_all_killer();
    me->set_temp("pending/hidden", 1 + (eff > 0 ? random(eff * 2) : 0));

    foreach(object ob in enemies) {
        ob->remove_killer(me);
        if( me->visible(ob) ) seen += ({ ob });
    }

    message("vision", HIK "煙散之後﹐原地已不見" + me->name() + "的蹤影。\n" NOR,
        environment(me), ({ me }) + seen);
    tell_object(me, HIK "你藉著煙霧斂去氣息﹐悄悄退出了戰圈。\n" NOR);
    foreach(object ob in seen)
        message_vision(HIK "$n冷笑一聲﹐目光穿過煙霧﹐牢牢盯住了$N。\n" NOR, me, ob, 1);
    return 1;
}

// ------------------------------------------------------------------
// 施咒
// ------------------------------------------------------------------

varargs int cast_spell(object me, string spell, object target)
{
    object scroll, *enemies;
    int sk, sen, wis, hit;

    if( me->skill_mapped("spells") != "taoism of darkness" )
        return notify_fail("你沒有使用茅山奇術。\n");
    if( me->is_busy() ) return notify_fail("你現在沒有空施咒。\n");
    if( !stringp(spell) || undefinedp(SPELLS[spell]) )
        return notify_fail("指令格式﹕cast <咒文名稱> [on <施咒對象>]\n");

    sk = me->query_skill("taoism of darkness", 1);
    /* 求教後還沒學成的 0 級也算學會，可以施展需求 0 級的鬼火術。 */
    if( (sk < 1 && me->query_learn("taoism of darkness") < 1) || sk < SPELLS[spell][0] )
        return notify_fail("你的茅山奇術火候未到﹐還驅使不了這道咒法。\n");

    if( spell != "smoke" ) {
        if( !objectp(target) ) {
            enemies = me->query_enemy();
            if( sizeof(enemies) ) target = enemies[0];
        }
        if( !objectp(target) || target == me || !living(target)
        ||  environment(target) != environment(me) )
            return notify_fail("這裡沒有可施咒的對象。\n");
    }

    if( !(scroll = find_scroll(me)) )
        return notify_fail("你身上沒有少陰符﹐無從施展奇術。\n");

    sen = me->query_stat("sen");
    wis = me->query_attr("wis");
    if( spell == "smoke" ) {
        if( !do_smoke(me) ) return 0;
        hit = 1;
    }
    scroll->add_amount(-1);
    // 每次施展得法術修為 random(慧根/6) + 1（比照天師咒術）。
    me->gain_score("magic", (wis >= 6 ? random(wis / 6) : 0) + 1);
    if( spell != "smoke" ) hit = do_fire(me, target, spell, sen);

    spell_gain(me, SPELLS[spell][2], hit);
    me->start_busy(SPELLS[spell][1]);
    return 1;
}
