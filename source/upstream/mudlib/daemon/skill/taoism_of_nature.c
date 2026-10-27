/*---
description: 茅山幻術（taoism of nature）。隱風觀專長咒術，enable 在咒術（spells）上，以 cast 施展；
             每次施展消耗一張先天符。dog 喚犬、monkey 喚猿、hydra 喚蟒、rain 祈天。
             設計見 docs/martial_arts/隱風觀_茅山幻術與遁甲書.md。
---*/
#include <ansi.h>

inherit SKILL;

#define SCROLL  "/custom/item/scroll/nature"
#define DOG     "/custom/taoism/npc/maoshan_dog"
#define FACTION "taoist.wind"

string type() { return "spell"; }
int valid_enable(string usage) { return usage == "spells"; }

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("taoism of nature");
    setup();
}

/* 學習條件：隱風觀弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != FACTION )
        return notify_fail("你不是隱風觀弟子﹐參不透茅山幻術的門道。\n");
    return 1;
}

/* 練成 20 級。 */
int query_entry_level() { return 20; }

void skill_completed(object me, string skill)
{
    tell_object(me, HIG "你默誦咒訣良久﹐指間符紙無火自燃﹐茅山幻術總算入了門。\n" NOR);
}

// ------------------------------------------------------------------
// 咒文資料（使用者定案，2026-10-22）
// ------------------------------------------------------------------

/* 咒文：可用等級、耗神、停頓、咒文重數（算點數用）。 */
mapping SPELLS = ([
    "dog":    ({  0, 10, 2, 1 }),
    "monkey": ({ 35, 20, 1, 2 }),
    "hydra":  ({ 55, 50, 3, 3 }),
    "rain":   ({ 90, 60, 1, 4 }),
]);

/* 施咒點數（照天師咒術）：
 * 基本咒術：((random(慧根)+1) × (random(慧根)+1) + random(慧根)) × (random(重數)+1)
 * 茅山幻術：(random(悟性) × random(慧根) + random(慧根)) × (random(重數)+1)
 * 沒成功只給 1/4、至少 1 點。 */
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
    if( exp > 0 ) me->improve_skill_exact("taoism of nature", exp);
}

object find_scroll(object me)
{
    foreach(object ob in all_inventory(me))
        if( base_name(ob) == SCROLL ) return ob;
    return 0;
}

/* 共同的施法前置：消耗先天符與神、停頓，並得法術修為 random(慧根/6) + 1。 */
void pay_cost(object me, object scroll, string spell)
{
    int wis;

    scroll->add_amount(-1);
    me->consume_stat("sen", SPELLS[spell][1]);
    me->start_busy(SPELLS[spell][2]);
    wis = me->query_attr("wis");
    me->gain_score("magic", (wis >= 6 ? random(wis / 6) : 0) + 1);
}

// ------------------------------------------------------------------
// 喚犬之術：強度 等級/30 + 1（最高 10），數量 等級/90 + 1；
// 重複召喚＝收回舊的、換一批新的。
// ------------------------------------------------------------------

// 召喚出的犬隻名字可能不同：同名的合在一起數，例如「兩條黃狗和一條白狗」；
// 只差顏色的（兩種靈獒）也算同名，直接說「三條靈獒」，用第一條的顏色。
string dog_list(object *dogs)
{
    string *kinds = ({}), *parts = ({});
    mapping num = ([]), shown = ([]);
    string kind;

    foreach(object dog in dogs) {
        kind = dog->query_dog_kind();
        if( undefinedp(num[kind]) ) {
            kinds += ({ kind });
            shown[kind] = dog->name();
        }
        num[kind]++;
    }
    foreach(kind in kinds)
        parts += ({ ({ "一", "兩", "三" })[num[kind] - 1] + "條" + shown[kind] });
    if( sizeof(parts) == 1 ) return parts[0];
    return implode(parts[0..<2], "、") + "和" + parts[<1];
}

int do_dog(object me, int sk)
{
    object dog, *dogs;
    int level, count, i;

    level = sk / 30 + 1;
    if( level > 10 ) level = 10;
    count = sk / 90 + 1;

    dogs = me->query_temp("maoshan/dogs");
    if( arrayp(dogs) )
        foreach(object old in dogs)
            if( objectp(old) ) old->dismiss();

    message_vision("$N捏起一張先天符往地上一擲﹐低喝一聲「敕」﹐符紙化作一團青煙。\n", me, 0, 1);
    dogs = ({});
    for( i = 0; i < count; i++ ) {
        dog = new(DOG);
        dog->set_summon(me, level);
        dog->move(environment(me));
        dogs += ({ dog });
    }
    me->set_temp("maoshan/dogs", dogs);
    message_vision("青煙散處竄出" + dog_list(dogs) + "﹐伏在$N腳邊低聲嗚嗚作響。\n", me, 0, 1);
    return 1;
}

// ------------------------------------------------------------------
// 喚猿之術：75% 機率讓對手停頓 2 回合；只有敘述，不召喚 NPC。
// ------------------------------------------------------------------

int do_monkey(object me, object target)
{
    me->fight_ob(target);
    target->fight_ob(me);
    message_vision("$N將先天符往空中一拋﹐符紙翻了幾個筋斗化成一隻靈猿﹐吱吱怪叫著撲到$n面前抓耳撓腮。\n",
        me, target, 1);
    if( random(100) < 75 ) {
        message_vision("$n被攪得眼花撩亂﹐一時竟忘了出手。\n", me, target, 1);
        target->start_busy(2);
        message_vision("靈猿一個翻身鑽進煙裡不見了。\n", me, target, 1);
        return 1;
    }
    message_vision("$n揮手將靈猿趕開﹐靈猿吱的一聲鑽進煙裡不見了。\n", me, target, 1);
    return 0;
}

// ------------------------------------------------------------------
// 喚蟒之術：只能對正在跟自己戰鬥的 NPC；random(等級/3) > 對手等級 就吞下；
// random(等級/10) > 10 時吐出被吞者身上的一件物品。
// ------------------------------------------------------------------

int do_hydra(object me, object target, int sk)
{
    object *items, item;

    message_vision("$N雙手結印將先天符按在地上﹐地面倏地裂開一道縫隙。\n", me, target, 1);
    if( random(sk / 3) <= target->query_level() ) {
        message_vision("巨蟒探出頭來嗅了嗅$n﹐似乎嫌對方太過扎手﹐又縮回地底去了。\n", me, target, 1);
        return 0;
    }

    message_vision(HIG "一條巨蟒自地底竄出﹐張開血盆大口將$n一口吞了下去﹗\n" NOR, me, target, 1);
    items = filter(all_inventory(target), (: !living($1) :));
    if( sizeof(items) && sk / 10 > 0 && random(sk / 10) > 10 ) {
        item = items[random(sizeof(items))];
        if( item->query("equipped") ) item->unequip();
        if( item->move(environment(me)) )
            message_vision("巨蟒喉頭一陣蠕動﹐吐出一" + (item->query("unit") ? item->query("unit") : "件")
                + item->name() + "﹐隨即沒入地底。\n", me, target, 1);
    }
    destruct(target);
    return 1;
}

// ------------------------------------------------------------------
// 祈天之術：房間內其他玩家（不含自己與 NPC）的精、氣、神，
// 目前值與受損的上限都回復 等級/10 + random(慧根 + 靈性)，三項相同。
// ------------------------------------------------------------------

int do_rain(object me, int sk)
{
    int n, r;

    r = me->query_attr("wis") + me->query_attr("spi");
    n = sk / 10 + (r > 0 ? random(r) : 0);
    message_vision("$N手捏法訣仰天長嘯﹐將先天符向空中一拋﹐霎時烏雲四合。\n", me, 0, 1);
    message_vision(HIC "一陣甘霖灑落下來﹐眾人只覺渾身舒暢﹐疲憊一掃而空。\n" NOR, me, 0, 1);
    foreach(object ob in all_inventory(environment(me))) {
        if( ob == me || !userp(ob) || !living(ob) ) continue;
        foreach(string st in ({ "gin", "kee", "sen" })) {
            ob->heal_stat(st, n);
            ob->supplement_stat(st, n);
        }
    }
    return 1;
}

// ------------------------------------------------------------------
// 施咒
// ------------------------------------------------------------------

varargs int cast_spell(object me, string spell, object target)
{
    object scroll, *enemies;
    int sk, hit;

    if( me->skill_mapped("spells") != "taoism of nature" )
        return notify_fail("你沒有使用茅山幻術。\n");
    if( me->is_busy() ) return notify_fail("你現在沒有空施咒。\n");
    if( !stringp(spell) || undefinedp(SPELLS[spell]) )
        return notify_fail("指令格式﹕cast <咒文名稱> [on <施咒對象>]\n");

    sk = me->query_skill("taoism of nature", 1);
    if( (sk < 1 && me->query_learn("taoism of nature") < 1) || sk < SPELLS[spell][0] )
        return notify_fail("你的茅山幻術火候未到﹐還驅使不了這道咒法。\n");

    if( spell == "monkey" || spell == "hydra" ) {
        if( !objectp(target) ) {
            enemies = me->query_enemy();
            if( sizeof(enemies) ) target = enemies[0];
        }
        if( !objectp(target) || target == me || !living(target)
        ||  environment(target) != environment(me) )
            return notify_fail("這裡沒有可施咒的對象。\n");
        if( spell == "hydra" ) {
            if( target->query("training_dummy") )
                return notify_fail("巨蟒對這木頭疙瘩毫無興趣。\n");
            if( userp(target) || !me->is_fighting(target) )
                return notify_fail("喚蟒之術只能對付正在與你交手的對象。\n");
        }
    }

    if( !(scroll = find_scroll(me)) )
        return notify_fail("你身上沒有先天符﹐無從施展幻術。\n");
    if( me->query_stat("sen") < SPELLS[spell][1] )
        return notify_fail("你心神渙散﹐念不完這段咒文。\n");

    pay_cost(me, scroll, spell);
    switch( spell ) {
    case "dog":    hit = do_dog(me, sk); break;
    case "monkey": hit = do_monkey(me, target); break;
    case "hydra":  hit = do_hydra(me, target, sk); break;
    case "rain":   hit = do_rain(me, sk); break;
    }
    spell_gain(me, SPELLS[spell][3], hit);
    return 1;
}
