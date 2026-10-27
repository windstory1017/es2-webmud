/*---
description: combat daemon, used to handle combat mechanics and messages.
author: Annihilator <taedlar@gmail.com>
---*/
#pragma save_binary

#include <ansi.h>
#include <combat.h>

//#define SHOW_DAMAGE
//#define SHOW_COMBAT_STAT

inherit F_CLEAN_UP;
inherit F_DBASE;

string *catch_hunt_msg = ({
    HIW "$N和$n仇人相見分外眼紅﹐立刻打了起來！\n" NOR,
    HIW "$N對著$n大喝﹕「可惡﹐又是你！」\n" NOR,
    HIW "$N和$n一碰面﹐二話不說就打了起來！\n" NOR,
    HIW "$N一眼瞥見$n﹐「哼」的一聲衝了過來！\n" NOR,
    HIW "$N喝道﹕「狗賊！別跑」\n" NOR,
    HIW "$N喝道﹕「納命來！」\n" NOR
});

/* 練武用的假人、木樁（set("training_dummy", 1)）被命中時的敘述：死物，不提部位、不提傷勢。 */
string *dummy_hit_msg = ({
    "結果扎扎實實的命中$n。\n",
    "結果不偏不倚地擊中$n。\n",
    "結果「啪」地一聲落在$n上，$n紋風不動。\n",
});

string *dead_msg = ({
    "\n$N死了。\n\n",
    "\n$N吐出幾口鮮血﹐抽搐了幾下 ... 死了。\n\n",
    "\n$N倒臥在血泊中﹐嘴角流出鮮血﹐眼看是不活了。\n\n",
    "\n$N呻吟了幾聲﹐咳出一口鮮血﹐便斷了氣。\n\n",
    "\n$N趴在地上﹐無力地掙扎了一會兒﹐隨即死去。\n\n",
});

private void create() {
    seteuid (getuid());
    set ("name", "戰鬥精靈");
    set ("id", "combatd");
}

string damage_msg (int damage, string type) {
    string str;

    if (!type) type = "傷害";
#ifdef SHOW_DAMAGE
    return "結果造成 " + damage + " 點" + type + "。\n";
#else
    if (damage == -1)
        switch (type) {
        case "瘀傷":
            return "結果「砰」地一聲打個正著。\n";
        case "割傷":
            return "結果「刷」地一聲砍出一道缺口。\n";
        case "刺傷":
            return "結果「噗」地一聲刺了進去。\n";
        default:
            return "結果直接命中$n。\n";
        }

    if (damage == 0)
        return "結果似乎沒有造成明顯的傷害。\n";
    if (damage > 100)
        damage = 100;

    switch (type) {
    case "斲傷":
    case "割傷":
        if (damage < 5) return "但是$w只輕輕地劃破$p的皮肉。\n";
        else if (damage < 10) return "不過僅在$p$l劃出一道細長的血痕。\n";
        else if (damage < 20) return "結果$w「嗤」地一聲劃出一道傷口！\n";
        else if (damage < 40) return "結果「嗤」地一聲劃出一道血淋淋的傷口！\n";
        else if (damage < 60) return "結果「嗤」地一聲劃出一道又長又深的傷口﹐濺得$N滿臉鮮血！\n";
        else return "只聽見$n一聲慘嚎﹐$w已在$p$l劃出一道深及見骨的可怕傷口！！\n";
        break;
    case "刺傷":
        if (damage < 5) return "但是$w只輕輕地刺破$p的皮肉。\n";
        else if (damage < 10) return "不過僅在$p$l刺出一個創口。\n";
        else if (damage < 20) return "結果$w「噗」地一聲刺入了$n$l寸許！\n";
        else if (damage < 40) return "結果「噗」地一聲刺進$n的$l﹐使$p不由自主地退了幾步！\n";
        else if (damage < 60) return "結果「噗嗤」地一聲﹐$w已在$p$l刺出一個血肉糢糊的血窟窿！\n";
        else return "結果只見$n瞪大雙眼﹐$w竟在$p的$l對穿而出﹐鮮血濺得滿地！！\n";
        break;
    case "瘀傷":
        if (damage < 2) return "儘管歪歪地擊中$l﹐但是似乎傷害不大。\n";
        else if (damage < 5) return "不過僅在$p的$l造成一處瘀青。\n";
        else if (damage < 10) return "結果命中$n的$l﹐登時腫了一塊老高！\n";
        else if (damage < 20) return "結果一擊命中﹐$n悶哼了一聲顯然吃了不小的虧！\n";
        else if (damage < 40) return "結果「砰」地一聲﹐$n不由自主地退了兩步！\n";
        else if (damage < 60) return "結果這一下「砰」地一聲打得$n連退了好幾步﹐差一點摔倒！\n";
        return "結果重重地擊中﹐震得$n「哇」地一聲吐出一口鮮血！！\n";
        break;
    default:
        if (damage < 2) str =  "結果只是勉強造成一處輕微";
        else if (damage < 5) str = "結果造成輕微的";
        else if (damage < 10) str = "結果造成一處";
        else if (damage < 20) str = "結果造成一處嚴重";
        else if (damage < 30) str = "結果造成頗為嚴重的";
        else if (damage < 40) str = "結果造成相當嚴重的";
        else if (damage < 60) str = "結果造成十分嚴重的";
        else if (damage < 80) str = "結果造成極其嚴重的";
        else str =  "結果造成非常可怕的嚴重";
        return str + type + "﹗\n";
    }
#endif
}

string
eff_status_msg (int ratio)
{
    if( ratio==100 ) return HIG "( $N看起來並沒有受傷。 )\n" NOR;
    if( ratio > 95 ) return HIG "( $N似乎受了點輕傷﹐不過光從外表看不大出來。 )\n" NOR;
    if( ratio > 90 ) return HIG "( $N看起來可能受了點輕傷。 )\n" NOR;
    if( ratio > 80 ) return HIY "( $N受了幾處傷﹐不過似乎並不礙事。 )\n" NOR;
    if( ratio > 60 ) return HIY "( $N受傷不輕﹐看起來狀況並不太好。 )\n" NOR;
    if( ratio > 40 ) return HIY "( $N氣息粗重﹐動作開始散亂﹐看來所受的傷著實不輕。 )\n" NOR;
    if( ratio > 30 ) return HIY "( $N已經傷痕累累﹐正在勉力支撐著不倒下去。 )\n" NOR;
    if( ratio > 20 ) return HIR "( $N受了相當重的傷﹐只怕會有生命危險。 )\n" NOR;
    if( ratio > 10 ) return HIR "( $N傷重之下已經難以支撐﹐眼看就要倒在地上。 )\n" NOR;
    if( ratio > 5  ) return HIR "( $N受傷過重﹐已經奄奄一息﹐命在旦夕了。 )\n" NOR;
    return HIR "( $N受傷過重﹐已經有如風中殘燭﹐隨時都可能斷氣。 )\n" NOR;
}

string
status_msg (int ratio)
{
    if( ratio==100 ) return HIG "( $N看起來對這種程度的攻擊一點也不在乎。 )\n" NOR;
    if( ratio > 95 ) return HIG "( $N似乎有些疲憊﹐但也許是誘敵之計﹐你無法確定。 )\n" NOR;
    if( ratio > 90 ) return HIG "( $N看起來可能有些累了﹐出招開始慢了下來。 )\n" NOR;
    if( ratio > 80 ) return HIY "( $N動作似乎開始有點不太靈光﹐但是仍然有條不紊。 )\n" NOR;
    if( ratio > 60 ) return HIY "( $N氣喘噓噓﹐看起來狀況並不太好。 )\n" NOR;
    if( ratio > 40 ) return HIY "( $N似乎十分疲憊﹐看來體力的消耗相當嚴重。 )\n" NOR;
    if( ratio > 30 ) return HIY "( $N已經一副頭重腳輕的模樣﹐正在勉力支撐著不倒下去。 )\n" NOR;
    if( ratio > 20 ) return HIR "( $N看起來已經力不從心了。 )\n" NOR;
    if( ratio > 10 ) return HIR "( $N搖頭晃腦、歪歪斜斜地站都站不穩﹐眼看就要倒在地上。 )\n" NOR;
    return HIR "( $N已經陷入半昏迷狀態﹐隨時都可能摔倒暈去。 )\n" NOR;
}

varargs void
report_status (object ob, int effective)
{
    if( !ob->query_stat_maximum("kee") ) return;
    if( effective ) 
        message_vision( eff_status_msg(
            (int)ob->query_stat_effective("kee") * 100 / (int)ob->query_stat_maximum("kee") ), ob);
    else
        message_vision( status_msg(
            (int)ob->query_stat("kee") * 100 / (int)ob->query_stat_maximum("kee") ) , ob);
}


/* User-restored combat skill learned formulas. */
private int restored_d(object me, object opponent)
{
    int d;
    d = opponent->query_level() - me->query_level();
    if( d < 2 ) d = 2;
    else if( d > 10 ) d = 10;
    return d;
}

/* 智力加成 智力/7，最低 1：智力不足 7 只是沒有加成，不會學不到技能。 */
private int restored_int_base(object me)
{
    int ib;
    ib = me->query_attr("int") / 7;
    return ib < 1 ? 1 : ib;
}

/* 防禦經驗：(random(屬性) + 1) × (random(d) + 1) × (智力/7)。
 * 閃躲用敏捷（dex），格擋用定力（cps）。 */
private int restored_defense_exp(object me, object opponent, string attr)
{
    int d, a;
    d = restored_d(me, opponent);
    a = me->query_attr(attr);
    return ((a > 0 ? random(a) : 0) + 1) * (random(d) + 1) * restored_int_base(me);
}

/* 成功閃躲、格擋時取得實戰經驗（combat）。
 * 對手是玩家：random(2)，即 0 或 1。
 * 對手等級不高於自己：1 點；高 n 級：1 + random(n)，n 最多 10。 */
private void defense_combat_gain(object me, object opponent)
{
    int n;

    if( userp(opponent) ) {
        n = random(2);
        if( n > 0 ) me->gain_score("combat", n);
        return;
    }

    n = opponent->query_level() - me->query_level();
    if( n > 10 ) n = 10;
    me->gain_score("combat", n > 0 ? 1 + random(n) : 1);
}

/* 閃躲成功時呼叫：基本閃躲與 enable 在閃躲上的身法，
 * 都是 (random(敏捷)+1) × (random(d)+1) × (智力/7)，兩者各自擲骰。 */
void restored_dodge_gain(object me, object opponent)
{
    string mapped;
    if( !userp(me) || !objectp(opponent) ) return;
    defense_combat_gain(me, opponent);
    me->improve_skill_exact("dodge", restored_defense_exp(me, opponent, "dex"));
    mapped = me->skill_mapped("dodge");
    if( stringp(mapped) && mapped != "dodge" )
        me->improve_skill_exact(mapped, restored_defense_exp(me, opponent, "dex"));
}

/* 格擋成功（含完全擋下）時呼叫：基本招架 = (random(定力)+1) × (random(d)+1) × (智力/7)；
 * 學過強力格擋時，強力格擋另得這次招架點數的 (定力×1.5～100)%。
 * enable 在招架上的武功不從格擋拿點數；武功檔有 parry_gains_exp() 傳回 1 時才給（各自擲骰）。
 * failed = 1：招架不及（見 F_COMBAT 的 parry_attack()），點數為正常的 1/4、至少 1，
 * 不給實戰經驗（使用者定案，2026-10-17）。 */
varargs void restored_parry_gain(object me, object opponent, int failed)
{
    string art;
    object daemon;
    int exp, lo;
    if( !userp(me) || !objectp(opponent) ) return;
    if( !failed ) defense_combat_gain(me, opponent);
    exp = restored_defense_exp(me, opponent, "cps");
    if( failed && (exp /= 4) < 1 ) exp = 1;
    me->improve_skill_exact("parry", exp);
    /* 強力格擋：這次招架點數 × (定力×1.5 ～ 100)%，定力 67 以上固定 100%。 */
    if( me->query_learn("heavy_parry") ) {
        lo = me->query_attr("cps") * 3 / 2;
        if( lo > 100 ) lo = 100;
        if( lo < 0 ) lo = 0;
        me->improve_skill_exact("heavy_parry", exp * (lo + random(101 - lo)) / 100);
    }
    art = me->skill_mapped("parry");
    if( stringp(art) && art != "parry" && objectp(daemon = SKILL_D(art))
    &&  function_exists("parry_gains_exp", daemon) && daemon->parry_gains_exp() )
    {
        exp = restored_defense_exp(me, opponent, "cps");
        if( failed && (exp /= 4) < 1 ) exp = 1;
        me->improve_skill_exact(art, exp);
    }
}

/* 沒有另外設定公式的技能一律用：(random(智力)+1) × (random(智力)+1) + random(智力)。 */
int int_formula_exp(object me)
{
    int i;
    i = me->query_attr("int");
    if( i < 1 ) i = 1;
    return (random(i) + 1) * (random(i) + 1) + random(i);
}

/* 基本攻擊技能：命中時不給武術造詣。 */
private string *basic_attack_skills = ({
    "unarmed", "blade", "twohanded blade", "secondhand blade",
    "sword", "twohanded sword", "secondhand sword",
    "axe", "twohanded axe", "secondhand axe",
    "pike", "twohanded pike", "secondhand pike",
    "staff", "twohanded staff", "secondhand staff",
    "blunt", "twohanded blunt", "secondhand blunt",
    "dagger", "secondhand dagger", "needle", "secondhand needle",
    "whip", "beast"
});

private void restored_hit_gain(object me, object victim, string skill, object weapon)
{
    int exp, str, cps;
    string base, equipped, art, force;
    object daemon;

    if( !userp(me) || !objectp(victim) ) return;

    /* 用 enable 的武功（非基本技能）造成傷害，每次 1 點武術造詣。 */
    if( member_array(skill, basic_attack_skills) < 0 )
        me->gain_score("martial art", 1);

    /* 空手打中、且內功設定成特殊內功時，基本內功另得
     * (random(膂力) × random(定力) + random(膂力)) × (智力/7，最低 1)。 */
    if( !objectp(weapon) && stringp(force = me->skill_mapped("force")) && force != "force" ) {
        str = me->query_attr("str");
        cps = me->query_attr("cps");
        exp = ((str > 0 ? random(str) : 0) * (cps > 0 ? random(cps) : 0)
            + (str > 0 ? random(str) : 0)) * restored_int_base(me);
        if( exp > 0 ) me->improve_skill_exact("force", exp);
    }

    /* 空手沒用武功：基本拳腳用力量公式。 */
    if( skill == "unarmed" ) {
        str = me->query_attr("str");
        if( str > 0 ) {
            exp = (random(str) + 1) * (random(str) + 1) + random(str);
            me->improve_skill_exact("unarmed", exp);
        }
        return;
    }

    /* 新版武功引擎做的武功（std/martial_art.c）：基本技能取武器裝備的位置。 */
    if( objectp(daemon = SKILL_D(skill))
    &&  function_exists("is_martial_art", daemon) ) {
        equipped = objectp(weapon) ? weapon->query("equipped") : 0;
        if( stringp(equipped) && equipped[0..6] == "weapon/" ) base = equipped[7..];
        else base = daemon->query_base_skill();
        art = skill;
    }
    // Every other weapon skill (axe, sword, pike, staff, blade, their
    // twohanded/secondhand variants, dagger, needle, blunt, whip, ...)
    // gains its own experience directly.
    else base = skill;

    /* 基本技能與武功本身都用智力公式，各自擲骰；瘋虎刀法本身另加 random(膽識)。 */
    if( stringp(base) ) me->improve_skill_exact(base, int_formula_exp(me));
    if( stringp(art) ) {
        exp = int_formula_exp(me);
        if( art == "tiger-blade" ) exp += random(me->query_attr("cor"));
        me->improve_skill_exact(art, exp);
    }
}

// 招式的元素追加傷害，見 fight()。
int elemental_extra(object me, object victim, string element)
{
    int extra, resist;

    extra = me->query_temp("apply/damage_vs_" + element);
    if( extra <= 0 ) return 0;
    resist = victim->query_temp("apply/armor_vs_" + element);
    if( resist < 0 ) resist = 0;
    extra = extra * 100 / (100 + resist);
    if( extra < 1 ) return 0;
    return victim->receive_damage(extra, me, me);
}

// fight()
//
// This function serves as a lowest level routine for combat system and
// is usually called from a daemon of combat skill such as unarmed, sword
// etc.

varargs int
fight (object me, object victim, string skill, mapping action, object weapon)
{
    int ability, strength, damage, gin_cost, force_bonus, dummy;
    string msg, force_skill;

    // 若在非戰區, 且戰鬥雙方都沒被arrest, 停止戰鬥 -Dragoon
    // 拿掉userp(me) check, mob也不準在非戰區出手攻擊 -dragoon
    if( environment(me)->query("no_fight") ) {
	if( !victim->query("unprotect_mark") 
	&& !me->query("unprotect_mark") ) return 0;
    }

    // 若戰鬥到一半斷線, 停止雙方戰鬥 -Dragoon
    if( ( userp(me) && !interactive(me) ) 
    || ( userp(victim) && !interactive(victim) ) ) return 0;

    me->set_combat_message(action["action"]);
    me->set_temp("last_action", action);

    /* 練武假人：必定命中，敘述不提部位與傷勢。耗精照一般公式
     * （沒有內功為 random(2)，有內功依功力提高）。 */
    dummy = victim->query("training_dummy");

    /* 力道
     */
    strength = me->query_strength("attack");

    /* 內力對戰鬥的影響:
     *
     * 力道增加: 技能 * 氣 * (功力百分比 + 招式係數)% * 4
     *           => 技能 100, 氣 1000, 十成功力, 無招式, 會增加 400Kg 力道
     * 需要氣: 所增加力道 / 500;
     * 消耗精: 隨機(1 到 1 + 所增加力道 / 30000)
     */
    gin_cost = 1;
    if( force_skill = me->skill_mapped("force") )
    {
        int ratio, kee_required, kee;

        if( !(ratio = me->query("force_ratio")) ) ratio = 75;

        force_bonus = me->query_skill("force") * me->query_stat("kee") 
                * (ratio + action["force"]) / 25;
        kee_required = force_bonus / 500;

        if( userp(me) && kee_required > 0 ) {
            kee = me->query_stat("kee");
            if( kee < kee_required )
                force_bonus = force_bonus * kee / kee_required;
        }

        /* 當使用內功﹐以極低的功力百分比戰鬥時﹐有機會 gin_cost = 0 */
        gin_cost = random(2 + force_bonus/30000);
    }

    /* 招式耗精比例（action["gin_pct"]，100 = 一般，0 = 不耗精），新版武功的特效用。 */
    if( !undefinedp(action["gin_pct"]) )
        gin_cost = gin_cost * action["gin_pct"] / 100;

    /* 命中率: 攻擊能力 + 攻擊技能 + 招式係數 */
    ability = me->query_ability("attack")
        + me->query_skill(skill)
        + action["attack"]
        ;

    /* 若攻擊對象對攻擊者所使用的技能也熟悉﹐則給予命中率的負面效應。 */
    ability -= random( victim->query_skill(skill) );

    /* 若攻擊對象上一個招式有防禦效果﹐則給予命中率影響。 */
    ability -= victim->query_temp("last_action/defense");

    /* 攻擊者消耗少許精力。 */
    if( userp(me) ) {
        if( me->query_stat("gin") > gin_cost ) {
            me->consume_stat("gin", gin_cost);
            strength += force_bonus;
        } else {
            // 當攻擊者精少於要求的精力時, 除了降低攻擊能力, 另
            // 外仍需扣精 -Dragoon
            me->consume_stat("gin", gin_cost/2 + 1);
            ability /= 2;
            strength /= 2;
        }
    }
    else strength += force_bonus;

    if( ability < 1 ) ability = 1;

    /* 要求攻擊對象進行防禦。 */
    me->set_temp("defend_message", 0);
    /* 招式帶 must_hit（例如三門齊開）時不讓對方閃躲或格擋，防具照樣減傷。 */
    if( !action["must_hit"] && !dummy
    &&  !victim->is_busy()
    &&  victim->defend(ability, strength, weapon ? weapon : me) )
    {
        string defend_msg;

        /* 防禦成功﹐將防禦的訊息加入戰鬥訊息。 */
        defend_msg = me->query_temp("defend_message");
        me->add_combat_message( "﹐" );
        me->add_combat_message( stringp(defend_msg) ? defend_msg
                : "但是沒中" );
        damage = -1;
    }
    else
    {
        string absorb_msg;

        /* 防禦失敗﹐給予攻擊對象吸收力道的機會。 */
        int pct, parry_result;

        me->set_temp("absorb_message", 0);
        victim->delete_temp("parry_result");
        if( !action["must_hit"] && !dummy )
            strength -= (int)victim->absorb(ability, strength,
                    weapon ? weapon : me);
        absorb_msg = me->query_temp("absorb_message");
        me->add_combat_message( "﹐" );
        /* 沒有格擋成功時（absorb_message 只在格擋成功時設定）顯示招架不及，
         * 避免玩家誤以為有格擋。 */
        me->add_combat_message( stringp(absorb_msg) ? absorb_msg
                : "$n招架不及");

        /* 招架結果（見 F_COMBAT 的 absorb()）：1 格擋減傷、2 完全擋下。 */
        parry_result = victim->query_temp("parry_result");

        /* 若力道未完全被吸收﹐則產生傷害。 */
        /* 產生傷害﹐則視攻擊方式給予少數精熟度 -Dragoon */
        /* 每次至少 1 點：1～悟性÷3（使用者指示，2026-10-24）。 */
        /* 新版武功的傷害百分比（action["damage_pct"]，100 = 不變），
         * 加上格擋減傷，由 inflict_damage() 在扣防具之前套用。 */
        pct = action["damage_pct"] > 0 ? action["damage_pct"] : 100;
        if( parry_result == 1 )
            pct = pct * (100 - victim->query_temp("parry_reduce")) / 100;
        if( pct != 100 )
            me->set_temp("martial_damage_pct", pct > 0 ? pct : 1);
        /* 招式帶 no_armor 時傷害不扣防具（例如愛霜恨雪）。 */
        if( action["no_armor"] ) me->set_temp("martial_no_armor", 1);
        if( parry_result == 2 ) {
            /* 完全擋下：不受傷，也不接傷害訊息。 */
            damage = -1;
        }
        else if( strength > 0 ) {
            if( weapon ) {
                damage = weapon->inflict_damage(strength, victim);
                // 武器命中並造成傷害後的特殊效果（例如藍涎刀上毒）。
                // 武器沒有定義 hit_ob() 時，這個呼叫什麼都不做。
                if( damage > 0 && objectp(victim) )
                    weapon->hit_ob(me, victim, damage);
                me->gain_score("weapon mastery", random(me->query_attr("int")/3) + 1);
            } else {
                damage = me->inflict_damage(strength, victim);
                me->gain_score("unarmed mastery", random(me->query_attr("int")/3) + 1);
            }
        }
        else damage = 0;
        me->delete_temp("martial_damage_pct");
        me->delete_temp("martial_no_armor");
    }

    /* 招式帶有元素屬性（action["element"]：fire、ice、lightning、wind）時，
     * 命中並造成傷害後，另外加上攻擊者裝備的同元素傷害力
     * （apply/damage_vs_<元素>），再以目標的同元素防禦力（armor_vs_<元素>）減免。
     * 不同元素的傷害力不會加成。 */
    if( damage > 0 && stringp(action["element"]) && objectp(victim) )
        damage += elemental_extra(me, victim, action["element"]);

    /* 成功命中後才取得 restored skill learned。 */
    if( damage > 0 ) restored_hit_gain(me, victim, skill, weapon);

    /* 將傷害程度的訊息加入戰鬥訊息。 */
    if( damage >= 0 ) {
#ifdef SHOW_DAMAGE
        me->add_combat_message("﹐" + damage_msg( damage, action["damage_type"] ));
#else
        int max, percent;

        max = victim->query_stat_effective("kee");
        if( max ) percent = damage * 100 / max;
        else percent = -1;      /* 非生物的傷害訊息 */
        me->add_combat_message("﹐" + damage_msg( percent, action["damage_type"]));
#endif
    }
    else me->add_combat_message("。\n");

#ifdef SHOW_COMBAT_STAT
    message_vision(sprintf(HIY "%s的攻擊：[技巧 %d, 力道 %.2fkg]\n" NOR,
        me->name(), ability, strength / 1000.0), me);
#endif

    /* 送出戰鬥訊息。 */
    msg = me->get_combat_message();
    /* 招式帶 brief 時只顯示出招敘述，不接防禦與傷害訊息，也不報體力狀態
     * （由武功自行處理，例如三門齊開）。 */
    if( action["brief"] ) msg = action["action"] + "\n";
    /* 招式帶 silent 時完全不顯示（連擊由武功自行顯示敘述與體力狀態）。 */
    if( action["silent"] ) msg = 0;
    /* 練武假人：只留出招敘述（拿掉部位），接一句簡短的命中敘述。 */
    if( dummy && stringp(msg) ) {
        msg = action["action"];
        msg = replace_string(msg, "$n的$l", "$n");
        msg = replace_string(msg, "$n$l", "$n");
        msg = replace_string(msg, "$l", "$n");
        msg += "﹐" + dummy_hit_msg[random(sizeof(dummy_hit_msg))];
    }
    if( stringp(msg) )
    {
        string *limbs = victim->query("limbs");

        if( !arrayp(limbs) || !sizeof(limbs) ) {
            msg = replace_string(msg, "$l", "身上");
        } else {
            msg = replace_string(msg, "$l", limbs[random(sizeof(limbs))]);
        }
        if( weapon ) msg = replace_string(msg, "$w", weapon->name());

        message_vision( msg, me, victim, 1);
        if( damage > 0 && !action["brief"] && !dummy ) report_status(victim);
    }

    // 武器攻擊被閃躲、格擋，或命中但力道被完全吸收（沒造成傷害）後的
    // 特殊效果（例如冥魔杖的特攻）。武器沒有定義 miss_ob() 時，這個呼叫什麼都不做。
    if( damage <= 0 && weapon && objectp(victim) )
        weapon->miss_ob(me, victim);

    /* 傳回非零值(傷害力)表示攻擊者可獲得經驗 */
    if( living(victim) ) {
        /* 對手是生物 */
        return damage;
    } else {
        /* 對手是非生物﹐攻擊者能力需小於 80 才有機會得到經驗 */
        // ability > 80 的限制有點過嚴, 同狀況下force exp卻無此限制
        // 略為放寬這裏的設限, 反正stake已經限制了頂多能練到那一級
        // -Dragoon 
        return random(500) > ability ? damage : 0;
    }
}

// auto_fight()
//
// This function is to start an automatically fight. Currently this is
// used in "aggressive", "vendetta", "hatred", "berserk" fight.

void
auto_fight (object me, object obj, string type)
{
    // Don't let NPC autofight NPC.
    if( !userp(me) && !userp(obj) ) return;

    // This call_out gives victim a chance to slip trough the fierce guys.
    call_out( "start_" + type, 0, me, obj);
}

// add check net connection of player, testing work or not -Dragoon
void
start_hatred (object me, object obj)
{
    if( !me || !obj ) return;   // Are we still alive(not becoming a corpse)?

    if( !living(me)                             // Are we capable for a fight?
    ||  (userp(obj) && !interactive(obj))       // Is player net dead?
    ||  environment(me)!=environment(obj)       // Are we still in the same room?
    ||  environment(me)->query("no_fight")      // Are we in a peace room?
    )   return;

    // We found our hatred! Charge!
    message_vision( catch_hunt_msg[random(sizeof(catch_hunt_msg))], me, obj);
    me->kill_ob(obj);
    if( userp(obj) ) obj->fight_ob(me);
    else obj->kill_ob(me);
    me->attack(obj);
}

void
start_vendetta (object me, object obj)
{
    if( !me || !obj ) return;   // Are we still exist( not becoming a corpse )?

    if( me->is_fighting(obj)    // Are we busy fighting?
    ||  !living(me)             // Are we capable for a fight?
    ||  (userp(obj) && !interactive(obj))       // Is player net dead?
    ||  environment(me)!=environment(obj)       // Are we still in the same room?
    ||  environment(me)->query("no_fight")      // Are we in a peace room?
    )   return;

    // We found our vendetta opponent! Charge!
    message_vision( catch_hunt_msg[random(sizeof(catch_hunt_msg))], me, obj);
    me->kill_ob(obj);
    if( userp(obj) ) obj->fight_ob(me);
    else obj->kill_ob(me);
    me->attack(obj);
}

void
start_aggressive (object me, object obj)
{
    int def;

    if( !me || !obj ) return;   // Are we still exist( not becoming a corpse )?

    if( me->is_fighting()       // Are we busy fighting?
    ||  !living(me)             // Are we capable for a fight?
    ||  (userp(obj) && !interactive(obj))       // Is player net dead?
    ||  environment(me)!=environment(obj)       // Are we still in the same room?
    ||  environment(me)->query("no_fight")      // Are we in a peace room?
    ||  obj->query("life_form") == "ghost"	// Only kill living -Dragoon
    ||  me->query_stat("kee") < obj->query_stat("kee")
    )   return;

    // let cor, cps, and command: agg work -dragoon
    def = (int)me->query_attr("cps");
    if( me->query_temp("ashura/aggressive") < 1 )
	def += (int)me->query_attr("cps");
    if( random(me->query_attr("cor")) < def ) return;

    /* 讓阿修羅族遇到其他玩家時﹐只打攻勢比自己強的 */
//  好像設反了?? 先取消 -dragoon
//    if( userp(me) && userp(obj) 
//    && obj->query_ability("intimidate") > me->query_ability("intimidate") )
//        return;

    // We got a nice victim! Kill him/her/it!!!
    message_vision( catch_hunt_msg[random(sizeof(catch_hunt_msg))], me, obj);
    me->kill_ob(obj);
    if( userp(obj) ) obj->fight_ob(me);
    else obj->kill_ob(me);
    me->attack(obj);
}

// 阿修羅與 NPC 相遇時主動出手：出手機率 = 100 - 定力 x 3（最低 30%）。
// 只用 fight，不用 kill：雙方都不會下死手，打輸只會昏倒，玩家可以 halt 停手。
void
start_ashura (object me, object obj)
{
    int chance;

    if( !me || !obj ) return;

    if( !userp(me) || userp(obj)                // 只有阿修羅玩家對 NPC
    ||  me->is_fighting()                       // Are we busy fighting?
    ||  !living(me) || !living(obj)             // Are we capable for a fight?
    ||  !interactive(me)                        // Is player net dead?
    ||  environment(me)!=environment(obj)       // Are we still in the same room?
    ||  environment(me)->query("no_fight")      // Are we in a peace room?
    ||  me->query("life_form") == "ghost"
    ||  obj->query("life_form") == "ghost"
    ||  !obj->visible(me)                       // 看不見的對象不會出手
    )   return;

    chance = 100 - (int)me->query_attr("cps") * 3;
    if( chance < 30 ) chance = 30;
    if( random(100) >= chance ) return;

    message_vision( catch_hunt_msg[random(sizeof(catch_hunt_msg))], me, obj);
    me->fight_ob(obj);
    obj->fight_ob(me);
    me->attack(obj);
}

// This function is to announce the special events of the combat.
// This should be moved to another daemon in the future.
void
announce (object ob, string event)
{
    switch(event)
    {
    case "dead":
        message_vision( dead_msg[random(sizeof(dead_msg))], ob );
        break;
    case "unconcious":
        message_vision("\n$N腳下一個不穩﹐跌在地上一動也不動了。\n\n", ob);
        break;
    case "revive":
        message_vision("\n$N慢慢睜開眼睛﹐清醒了過來。\n\n", ob);
        break;
    }
}

