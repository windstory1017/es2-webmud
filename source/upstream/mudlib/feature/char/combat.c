/*---
description: 角色戰鬥相關功能的定義。
author: Annihilator <taedlar@gmail.com>
---*/

#include <attack.h>
#include <dbase.h>
#include <score.h>
#include <skill.h>
#include <statistic.h>
#include <combat.h>

static string combat_message = "";

void set_combat_message(string s) { combat_message = s; }
void add_combat_message(string s) { combat_message += s; }
string get_combat_message() { return combat_message; }

varargs int query_attr(string,int);

// query_strength()
//
// This function returns the strength an character can bestow on specific
// application in the UNIT OF GRAM. Currently the default definition 
// implements a convert based on human attributes. Human strength ranged
// from 13 to 18, yielding the strength range from 16.9 kg to 32.4 kg.

int
query_strength (string application)
{
    if( !living(this_object()) ) return 0;
    switch(application) {
    case "attack":	// Strength to perform an attack.
	return query_attr("str") * query_attr("cor") * 150
		+ query_stat("kee") * 30;
    case "defense":	// Strength to defense against attack.
	return query_attr("str") * query_attr("cps") * 150
		+ query_stat("kee") * 30;
    case "magic":	// Strength of magic power
	return query_attr("spi") * query_attr("spi") * 150
		+ query_stat("sen") * 30;
    case "spell":	// Strength of spell power
	return query_attr("wis") * query_attr("wis") * 150
		+ query_stat("sen") * 30;
    case "carriage":	// Strength to carry items.
	return query_attr("str") * query_attr("str") * 200;
    default:
	return 0;
    }
}

// query_ability()
//
// This function returns the ability measurement of the character's specific
// ability. Currently the value of ability had been adjusted to fit the skill
// system that with 100 as regular-maximum. Under this asumption, a human's
// natural attack ability ranges from 21 to 70 (an ultimate wimpy in dying
// status versus an top ace in perfect condition) in the UNIT OF SKILL LEVEL.

int
query_ability (string application)
{
    if( !living(this_object()) ) return 0;

    switch(application) {

    /* attack :
     *
     * 實體攻擊的基本技巧值。
     */
    case "attack":
	return query_attr("dex") * query_attr("cor") / 10
		+ (query_stat("gin") >> 5)
		+ query_temp("apply/attack");

    /* defense :
     *
     * 對實體攻擊的基本防禦值。
     */
    case "defense":
	return query_attr("dex") * query_attr("cps") / 10
		+ (query_stat("gin") >> 5)
		+ query_temp("apply/defense");

    /* intimidate:
     *
     * 攻勢等級。
     */
    case "intimidate":
	return query_attr("cor") * query_attr("str") / 10
		+ (query_stat("kee") >> 5)
		+ query_temp("apply/intimidate");

    /* wittiness:
     *
     * 守勢等級。
     */
    case "wittiness":
	return query_attr("cps") * query_attr("wis") / 10
		+ (query_stat("kee") >> 4)
		+ query_temp("apply/wittiness");

    /* magic:
     *
     * 魔力。
     */
    case "magic":
	/* MODIFIED (WebMUD): the extra term uses its own key so it does not
	 * also raise the "magic" skill level (query_skill reads apply/<skill>). */
	return query_attr("spi") * query_attr("int") / 10
		+ (query_stat("sen") >> 5)
		+ query_temp("apply/magic_ability");

    /* spell:
     *
     * 法力。
     */
    case "spell":
	return query_attr("spi") * query_attr("wis") / 10
		+ (query_stat("sen") >> 5)
		+ query_temp("apply/spell");

    case "move":
	return query_attr("str") * query_attr("dex") / 10
		+ (query_stat("gin") >> 5)
		+ query_temp("apply/move");

    /* awarness:
     *
     * 對隱藏事物的感應力。
     */
    case "awarness":
	return query_attr("cps") * query_attr("spi") / 10
		+ (query_stat("gin") >> 4)
		+ query_temp("apply/awarness");

    case "cauterization":
	return query_attr("cor") * query_attr("wis") / 10
		+ query_stat("gin") / 40;
    default:
	return query_skill(application);
    }
}

/*
 * int defend(int ability, int strength, object from)
 *
 * 這個函數定義一個人物的「防禦」能力。
 *
 * 當人物受到某種攻擊的時候，應該呼叫這個函數讓人物有防禦的機會。
 * ability 及 strength 分別描述這個攻擊的技巧性及強度，from 表示這個攻擊
 * 所使用的物件，可能是另一個人物、武器、魔法技能等。
 *
 * defend 若傳回一個非 0 的數值，表示這個攻擊被成功地防禦住，這次的攻擊
 * 應該無效。若傳回 0 表示這個人物無法防禦，或防禦失敗。
 */

private void defense_gin_cost(string key, string attr);

int
defend (int ability, int strength, object from)
{
    string sk;
    int counter_ability, chance;

    if( !living(this_object()) ) return 0;

    switch(from->query("phase")) {
    case PHASE_MENTAL:
    case PHASE_ILLUSION:
    case PHASE_ELEMENTAL:
	counter_ability = query_ability("defense");
	break;
    case PHASE_PHYSICAL:
	/* 玩家停用閃躲（disable dodge）時完全不閃躲：這一刀一定打中，
	 * 直接進入招架判定。NPC 不受影響。 */
	if( userp(this_object()) && !skill_mapped("dodge") ) return 0;
	counter_ability = query_ability("defense");
	if( (sk=skill_mapped("dodge")) )
	    counter_ability += SKILL_D(sk)->dodge_using(this_object(),
			ability, strength, from);
	break;
    default:	/* unknown phase */
	return 0;
    }

    chance = 50 + (ability - counter_ability) * 2;
    if( chance < 5 ) chance = 5;
    else if( chance > 95 ) chance = 95;
    if( random(100) > chance ) {
        if( stringp(sk) && from->is_character() )
            COMBAT_D->restored_dodge_gain(this_object(), from);
        else if( stringp(sk) && environment(from) && environment(from)->is_character() )
            COMBAT_D->restored_dodge_gain(this_object(), environment(from));
        defense_gin_cost("dodge_gin_count", "dex");
        return 1;
    }
    return 0;
}

/*
 * int absorb(int ability, int strength, object from)
 *
 * 這個函數定義一個人物「承受」攻擊的能力。
 * ability 及 strength 表示這個攻擊的技巧性及強度，from 表示這個攻擊所使用的
 * 物件。
 *
 * absorb 應該傳回一個整數值，表示有多少攻擊的強度被「吸收」了。
 */

/* 閃躲、格擋成功的精神耗損（設計見 docs/COMBAT_DEFENSE_GIN.md）
 * 每成功 N 次扣 1 點精，N = 1 + 屬性 / 7（無條件捨去）。
 * 閃躲看敏捷（dex）、格擋看定力（cps），次數分開累計；玩家與 NPC 都適用。
 * 招架不及不算。不顯示任何訊息。精歸零後的昏倒由 statistic 的 exhausted 機制處理。 */
private void defense_gin_cost(string key, string attr)
{
    int n, c;

    n = 1 + query_attr(attr) / 7;
    c = query_temp(key) + 1;
    if( c >= n ) {
        set_temp(key, 0);
        consume_stat("gin", 1, this_object());
    }
    else set_temp(key, c);
}

private int parry_attack(int strength, object from);

int
absorb (int ability, int strength, object from)
{
    string sk;

    /* 鬼魂永遠吸收掉任何攻擊的所有的力道 */
    if( query("life_form")=="ghost" ) {
	if( from->is_character() )
	    from->add_combat_message("但是從$n身上穿透過去，");
	else if( environment(from) && environment(from)->is_character() )
	    environment(from)->add_combat_message("但是$w從$n身上穿透過去，");
	return strength;
    }

    /* 鬼魂的力道也永遠被完全吸收 */
    if( from->is_character() && from->query("life_form")=="ghost" ) {
	from->add_combat_message("但是陷進$n的身體，");
	return strength;
    }

    if( !living(this_object()) ) return 0;

    return parry_attack(strength, from);
}

/* 招架（NEW，規則見 docs/martial_arts/招架規則設計.md）
 *
 * 格擋力道上限 =（防守力道 + 自己的內功加成）×（100 + 招架等級）%，
 * 每一刀從 1 到上限隨機取值，跟對方這一刀的攻擊力道比：
 *   超過對方         → 格擋，傷害減少（1 + 招架等級 / 20）%
 *   達到對方 2.5 倍  → 完全擋下，不受傷
 * 學過重擋（heavy_parry）時格擋力道上限放大：對方有內功 ×5，沒有 ×2。
 * 空手也可以格擋。招架等級沿用 query_skill("parry")（有 enable 招架武功時取平均）。
 * 結果記在自己的 temp：parry_result（1 格擋、2 完全擋下）、parry_reduce（減傷 %），
 * 由 COMBAT_D->fight() 讀取。
 * enable 在招架上的武功可以實作：
 *   string parry_message(object me, object attacker, int full)  拆招敘述
 *   void parry_success(object me, object attacker, int full)    格擋後的特效
 */
private int parry_force_bonus()
{
    int ratio, bonus, kee, kee_required;

    if( query_skill("force") <= 0 ) return 0;
    if( !(ratio = query("force_ratio")) ) ratio = 75;
    bonus = query_skill("force") * query_stat("kee") * ratio / 25;

    // 跟攻擊方一樣：氣不夠時內功加成按比例打折。
    kee_required = bonus / 500;
    kee = query_stat("kee");
    if( kee_required > 0 && kee < kee_required )
        bonus = bonus * kee / kee_required;
    return bonus;
}

private int parry_attack(int strength, object from)
{
    object attacker, daemon;
    string art, msg;
    int sk, max, block, full, reduce;

    if( strength < 1 ) return 0;

    attacker = from->is_character() ? from : environment(from);

    sk = query_skill("parry");
    max = (query_strength("defense") + parry_force_bonus()) * (100 + sk) / 100;

    // 重擋(heavy_parry，軍人技巧)：格擋力道上限放大，對方有內功時 ×5，沒有時 ×2。
    if( query_learn("heavy_parry") )
        max *= (objectp(attacker) && attacker->query_skill("force") > 0) ? 5 : 2;
    if( max < 1 ) return 0;

    block = 1 + random(max);
    if( block <= strength ) {
        // 保底格擋：力道比不過時，仍有 5% + 招架等級/4 %（最多 50%）的機會
        // 部分格擋（不會完全擋下）；沒抽中就是招架不及，給 1/4 招架經驗
        // （使用者定案，2026-10-17）。
        if( random(100) >= (5 + sk / 4 > 50 ? 50 : 5 + sk / 4) ) {
            if( objectp(attacker) && attacker->is_character() )
                COMBAT_D->restored_parry_gain(this_object(), attacker, 1);
            return 0;
        }
        full = 0;
    }
    else full = block * 2 >= strength * 5;
    reduce = 1 + sk / 20;

    defense_gin_cost("parry_gin_count", "cps");
    set_temp("parry_result", full ? 2 : 1);
    set_temp("parry_reduce", reduce);

    art = skill_mapped("parry");
    if( stringp(art) && art != "parry" ) daemon = SKILL_D(art);

    if( objectp(daemon) && function_exists("parry_message", daemon) )
        msg = daemon->parry_message(this_object(), attacker, full);
    if( !stringp(msg) )
        msg = full ? "$n將$N這一擊格開" : "$n奮力格擋﹐卸去了$N這一擊的部分力道";
    if( objectp(attacker) ) {
        attacker->set_temp("absorb_message", msg);
        if( attacker->is_character() )
            COMBAT_D->restored_parry_gain(this_object(), attacker);
    }

    if( objectp(daemon) && function_exists("parry_success", daemon) )
        daemon->parry_success(this_object(), attacker, full);

    return full ? strength : 0;
}

// inflict_damage()
//
// This is the default function to inflict damage when unarmed. (If armed
// with weapon(s), the inflict_damage() in weapon object is called instead)
// For more information about the task of this function, see F_EQUIP for
// the comments of inflict_damage().

int
inflict_damage (int strength, object victim)
{
    int damage;

    // If we are not living, make no damage.
    if( !living(this_object()) ) return 0;

    // 傷害一律用「基本力道＋內功加成」換算（內功加成已在 COMBAT_D->fight()
    // 加進 strength）。原本有內功等級時改請內功技能計算傷害，但沒有任何內功
    // 實作 inflict_damage()，結果練了內功的人徒手傷害永遠是 0。
    damage = 1 + strength/10000 + random(strength/10000);
    damage += query_temp("apply/damage");

    // 新版武功的傷害百分比（見 COMBAT_D->fight()），在扣防具之前套用。
    if( query_temp("martial_damage_pct") > 0 )
        damage = damage * query_temp("martial_damage_pct") / 100;

    // Call victim to resist this.
    if( !query_temp("martial_no_armor") )
        damage -= victim->resist_damage(damage, this_object());

    return damage > 0 ? victim->receive_damage(damage, this_object(), this_object()) : 0;
}

// resist_damage()
//
// This function returns the non-initiative defense against all physical
// damage to this object. 

int
resist_damage (int damage, object from_ob)
{
    return query_temp("apply/armor");
}

// receive_damage()
//
// This function does actual damage on this object and return the actual
// number of damage received. Note that this function is for 'game system
// abstraction' reason and ANY DEFENSE should be defined in resist_damage
// instead. One example application  of this function is to convert hp/mp
// statistics system into gin/kee/sen statistic system.

varargs int
receive_damage (int damage, object from, object attacker)
{
    int max, kill_mode = 0;
    mapping flag;
    string from_type;

    if( damage < 1 ) return 0;
    if( !living(this_object()) ) damage *= 10;

    kill_mode = objectp(attacker) ? attacker->is_killing(this_object()) : 0;
    from_type = objectp(from) ? from->query("material") : "unknown";

    // Damage this character according the life form it is in.
    // Currently the default form is living, which is, damage kee.
    switch( query("life_form") ) {
    case "ghost":
	consume_stat("gin", damage, attacker);

	if( kill_mode )
	{
	    if( living(this_object()) ) {
		/* 鬼魂以精保護自己 */
		max = query_stat_effective("gin");
		if( random(max/3) < damage ) {
		    damage_stat("gin", damage, attacker);
		}

		/* 如果傷害力和精越接近，則傷害形體的機會越高 */
		if( random(query_stat("gin")) < damage )
		    consume_stat("HP", random(damage)+1, attacker);
	    } else {
		/* 人物呈昏迷狀態時，直接由形體承受所有傷害 */
		consume_stat("HP", damage, attacker);
	    }
	}
	break;
    case "living":
    default:
	/* 消耗氣力 */
	consume_stat("kee", damage, attacker);

	/* 只有 kill 模式才會受傷 */
	if( kill_mode )
	{
	    if( living(this_object()) ) {
		/* 氣的作用：保護人物不受傷害，所以當氣的狀態值越高時
		 * 受傷的機會越低，反之越高。
		 */
		max = query_stat_effective("kee");
		if( random(max/3) < damage ) {
		    damage_stat("kee", damage, attacker);
		    gain_score("survive", damage);
		}

		/* 如果傷害力和氣越接近，則傷害形體的機會越高 */
		if( random(query_stat("kee")) < damage )
		    consume_stat("HP", random(damage)+1, attacker);
	    } else {
		/* 人物呈昏迷狀態時，直接由形體承受所有傷害 */
		consume_stat("HP", damage, attacker);
	    }
	}
	break;
    }

    // Quick recover of character, occurs on the following condition:
    //
    // 1. Attacker is not killing this character.
    // 2. This character is not killing ANYONE.
    //    (And nobody is killing this character, however, this costs too much
    //    code to check, so let give it a chance in next turn.)
    // 3. This character is exhausting in next turn.
    //
    if(	!kill_mode 
    &&	!is_killing()
    &&	mapp(flag = query_exhausted())
    &&	(flag["gin"] || flag["kee"] || flag["sen"] || flag["HP"]) )
    {
        string stat;
        object ob;

	// fix all exhausted stats to zero.
        foreach(stat, ob in flag)
            set_stat_current(stat, 1);

	clear_statistic_flags();

        // ask all enemy to stop, they've done enough.
        remove_all_enemy();

        call_out("lose_fight", 0, attacker);
        return damage;
    }

    return damage;
}

