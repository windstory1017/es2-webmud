

#include <ansi.h>
#include <dbase.h>
#include <skill.h>

mapping skills          = ([]);         // The level of skills.
mapping learned         = ([]);         // The learning progress of skills.
mapping skill_map       = ([]);         // The links of mapped skill.
mapping skill_flag      = ([]);         // Ultra skill flags.

string best_skill = "";

/* Old Neolith LPC needs inherited function prototypes visible at compile time. */
varargs int query_attr(string attr, int raw);
int set_attr(string what, int value);
varargs void advance_skill(string skill, int amount);
void apply_restored_skill_progression(string skill);
int query_personal_skill_cap(string skill);

// implementations

/* 武功代碼更名（使用者決定，2026-10-14）：舊存檔讀進來後把舊代碼換成新代碼，
 * 等級、修練進度、技能對應都保留。由 /feature/save.c 的 restore() 呼叫。 */
static mapping RENAMED_SKILL = ([
    "sanmeendo":   "ksanmando",     // 三門刀法
    "tiger-force": "tiger force",   // 瘋虎功
]);

void migrate_skill_ids()
{
    string old, nw, k;

    foreach( old, nw in RENAMED_SKILL ) {
        if( mapp(skills) && !undefinedp(skills[old]) ) {
            if( skills[nw] < skills[old] ) skills[nw] = skills[old];
            map_delete(skills, old);
        }
        if( mapp(learned) && !undefinedp(learned[old]) ) {
            if( learned[nw] < learned[old] ) learned[nw] = learned[old];
            map_delete(learned, old);
        }
        if( mapp(skill_flag) && !undefinedp(skill_flag[old]) ) {
            if( undefinedp(skill_flag[nw]) ) skill_flag[nw] = skill_flag[old];
            map_delete(skill_flag, old);
        }
        if( mapp(skill_map) ) {
            if( !undefinedp(skill_map[old]) ) {
                if( undefinedp(skill_map[nw]) ) skill_map[nw] = skill_map[old];
                map_delete(skill_map, old);
            }
            foreach( k in keys(skill_map) )
                if( skill_map[k] == old ) skill_map[k] = nw;
        }
        if( best_skill == old ) best_skill = nw;
    }
}

string query_best_skill()
{
    return best_skill;
}

mapping query_skills()
{
    if( previous_object() && geteuid(previous_object()) != ROOT_UID )
	return copy(skills);
    return skills;
}

mapping query_skill_flags()
{
    if( previous_object() && geteuid(previous_object()) != ROOT_UID )
	return copy(skill_flag);
    return skill_flag;
}

mapping query_learned()
{
    if( previous_object() && geteuid(previous_object()) != ROOT_UID )
	return copy(learned);
    return learned;
}

mapping query_skill_map()
{
    if( previous_object() && geteuid(previous_object()) != ROOT_UID )
	return copy(skill_map);
    return skill_map;
}

// set_skill()
//
// Sets the level of specific skill.
// Note that currently this function doesnot do any security check.
// Security check could be added in the future.

void set_skill(string skill, int val)
{
    if( val > 200 ) val = 200;
    skills[skill] = val;
    query_personal_skill_cap(skill);   // 得到技能的當下決定個人上限
}

int set_learn(string skill, int lrn)
{
    return learned[skill] = lrn;
}

// delete_skill()
//
// This function deletes a skill completely, including its learning progress
// and mapped reference.

int delete_skill(string skill)
{
    string s1, s2;

    map_delete(skills, skill);
    map_delete(learned, skill);
    foreach(s1, s2 in skill_map)
	if( s2==skill ) map_delete(skill_map, s1);

    return 1;
}

// map_skill()
//
// This function maps a skill to another. If no mapped_to specificed, or
// map one skill to itself, the skill is unmapped. If mapped_to is "none",
// the skill is 'disabled' that query_skill() always return 0 upon querying
// this skill.

varargs void map_skill(string skill, string mapped_to)
{
    if( !mapped_to ) {
	map_delete(skill_map, skill);
	return;
    }

    if( mapped_to=="none" ) skill_map[skill] = 0;
    else if( mapped_to==skill ) map_delete(skill_map, skill);
    else skill_map[skill] = mapped_to;
}

// skill_mapped()
//
// Return the skill name which the specific skill is mapped to. If the
// skill is not mapped, the name of the skill is returned.

string skill_mapped(string skill)
{
    return !undefinedp(skill_map[skill]) ? skill_map[skill] : skill;
}

// query_skill()
//
// Return the level of a skill. If the skill is mapped, the AVERAGE of
// original skill and mapped skill is return. If the skill is mapped to
// 0, (i.e. the skill is disabled, see map_skill()), 0 is returned.
varargs int query_skill(string skill, int raw)
{
    int s;

    if( raw ) return skills[skill];

    s = query_temp("apply/" + skill);
    if( undefinedp(skill_map[skill]) ) s += skills[skill];
    else {
	if( skill_map[skill]==0 ) return 0;
	s += (skills[skill] + query_skill(skill_map[skill]) ) / 2;
    }
    return (s > 200) ? 200 : s;
}

int query_learn(string skill)
{
    return learned[skill];
}

// skill gain:
//
// This mapping registers every improve_skill from last reset_skill_gain()
// till now.
static mapping skill_gain = ([]);

mapping query_skill_gain() { return skill_gain; }
void reset_skill_gain() { skill_gain = ([]); }

/* 技能升級規則（所有技能共用）
 *
 * 拿到技能點數只會累積在 learned，當下不升級。玩家下 gain 時，這段時間
 * 有拿到點數的技能才判斷：累積點數到了下一級的門檻就升一級，一次 gain
 * 最多一級。還沒學成的技能（0 級）要累積到學成等級的門檻，gain 時才直接
 * 學成到那一級，例如瘋虎功 20 級、龍圖心經 40 級；沒指定的技能學成等級是 1。
 * NPC 不會下 gain，拿到點數就照同一套門檻升級。
 *
 * 門檻：升到第 level 級所需的累積點數 = level² × base，越高級 base 越大。
 * 所有技能上限 200 級。
 */
int restored_skill_threshold(int level)
{
    int base;

    if( level < 1 ) return 0;
    if( level <= 60 ) base = 100;
    else if( level <= 90 ) base = 125;
    else if( level <= 120 ) base = 150;
    else if( level <= 160 ) base = 175;
    else if( level <= 180 ) base = 200;
    else base = 250;

    return level * level * base;
}

/* 個人技能上限（使用者定案，2026-10-22）：玩家得到一項技能時，隨機決定這項技能的上限
 * 140、145……200（逢 5，機率平均），記在 skill_cap/<技能>，之後不再改變。
 * 舊角色已經練過頭的技能，上限設成目前等級（不會掉級）。NPC 不受影響（200）。 */
int query_personal_skill_cap(string skill)
{
    int cap;

    if( !userp(this_object()) ) return 200;
    if( (cap = this_object()->query("skill_cap/" + skill)) > 0 ) return cap;
    cap = 140 + random(13) * 5;
    if( skills[skill] > cap ) cap = skills[skill];
    this_object()->set("skill_cap/" + skill, cap);
    return cap;
}

/* 技能等級上限：個人上限；技能檔有 query_skill_cap(人物) 時取兩者較低的
 * （例如水雲劍法暫時 30，使用者定案，2026-10-20）。 */
int restored_skill_cap(string skill)
{
    object daemon;
    int cap, own;

    own = query_personal_skill_cap(skill);
    daemon = SKILL_D(skill);
    if( objectp(daemon) && function_exists("query_skill_cap", daemon)
    &&  (cap = call_other(daemon, "query_skill_cap", this_object())) > 0 )
        return cap < own ? cap : own;
    return own;
}

// 學成等級：技能檔用 query_entry_level() 指定，沒指定的是 1。
int skill_entry_level(string skill)
{
    object daemon;
    int lv;

    daemon = SKILL_D(skill);
    if( objectp(daemon) && function_exists("query_entry_level", daemon) )
        lv = call_other(daemon, "query_entry_level");
    return lv > 0 ? lv : 1;
}

/* 個別技能的門檻：技能檔用 query_threshold_percent() 把基本門檻打折，
 * 例如龍圖心經 10（練滿 200 級只要基本門檻的十分之一）。沒指定的是 100。 */
int skill_threshold(string skill, int level)
{
    object daemon;
    int pct, entry;

    daemon = SKILL_D(skill);
    /* 學成門檻：技能檔用 query_entry_threshold() 指定學成那一級所需的累積點數，
     * 例如瘋虎刀法 50000 點學成 30 級；沒指定的照原本算法。 */
    if( objectp(daemon) && function_exists("query_entry_threshold", daemon)
    &&  level == skill_entry_level(skill)
    &&  (entry = call_other(daemon, "query_entry_threshold")) > 0 )
        return entry;

    if( objectp(daemon) && function_exists("query_threshold_percent", daemon) )
        pct = call_other(daemon, "query_threshold_percent");
    if( pct <= 0 ) pct = 100;
    return restored_skill_threshold(level) * pct / 100;
}

// 累積點數夠升的下一個等級；還不夠就傳回 0。
int skill_next_level(string skill)
{
    int level, next;

    level = skills[skill];
    if( level >= restored_skill_cap(skill) ) return 0;
    next = level ? level + 1 : skill_entry_level(skill);
    if( learned[skill] < skill_threshold(skill, next) ) return 0;
    return next;
}

private void raise_skill_to(string skill, int next)
{
    object daemon;

    // 從 0 級直接學成：先讓技能檔顯示練成的敘述、給學成獎勵。
    if( !skills[skill] && next > 1 ) {
        daemon = SKILL_D(skill);
        if( objectp(daemon) && function_exists("skill_completed", daemon) )
            call_other(daemon, "skill_completed", this_object(), skill);
    }
    advance_skill(skill, next - skills[skill]);
}

// gain 指令呼叫：gained 是這段時間拿到點數的技能。
void apply_gain_progression(mapping gained)
{
    string skill;
    int amount, next;

    if( !mapp(gained) ) return;
    foreach(skill, amount in gained) {
        if( amount <= 0 ) continue;
        if( skill_flag[skill] & SKILL_FLAG_ABANDONED ) continue;
        next = skill_next_level(skill);
        if( next ) raise_skill_to(skill, next);
    }
}

// 拿到點數當下的升級，只對 NPC 生效；玩家要等 gain。
void apply_restored_skill_progression(string skill)
{
    int next;

    if( userp(this_object()) ) return;
    while( 1 ) {
        next = skill_next_level(skill);
        if( !next ) break;
        raise_skill_to(skill, next);
    }
}

/* Exact learned gain + old-Neolith-compatible daemon hook. */
void improve_skill_exact(string skill, int amount)
{
    object daemon;

    if( skill_flag[skill] & SKILL_FLAG_ABANDONED ) return;
    if( amount <= 0 ) return;
    // 技能已達個人上限（天賦值）就不再累積點數
    if( userp(this_object()) && skills[skill] > 0
    &&  skills[skill] >= restored_skill_cap(skill) ) return;
    /* 人物等級加成：點數 × (1 + random(人物等級 / 8))。 */
    amount += amount * random(this_object()->query_level() / 8);
    if( undefinedp(learned[skill]) ) learned[skill] = amount; else learned[skill] += amount;
    if( undefinedp(skill_gain[skill]) ) skill_gain[skill] = amount; else skill_gain[skill] += amount;
    if( undefinedp(skills[skill]) ) skills[skill] = 0;
    query_personal_skill_cap(skill);   // 得到技能的當下決定個人上限

    daemon = SKILL_D(skill);
    if( objectp(daemon) )
    {
        if( function_exists("skill_improved", daemon) )
            call_other(daemon, "skill_improved", this_object(), skill);
    }

    apply_restored_skill_progression(skill);
}

int restored_force_tick_exp()
{
    int con, cps, ib;
    con = query_attr("con");
    cps = query_attr("cps");
    ib = query_attr("int") / 7;
    if( ib < 1 ) ib = 1;    // 智力不足 7 只是沒有加成
    return (con > 0 ? random(con) : 0) + (cps > 0 ? random(cps) : 0) * ib;
}

void improve_restored_force_tick()
{
    improve_skill_exact("force", restored_force_tick_exp());
}

// improve_skill()
//
// This function improves the skill by adding specific amount to its
// learning progress.

varargs void
improve_skill(string skill, int amount)
{
    if( skill_flag[skill] & SKILL_FLAG_ABANDONED ) return;

    if( !amount ) amount = 1;
    // 技能已達個人上限（天賦值）就不再累積點數
    if( userp(this_object()) && skills[skill] > 0
    &&  skills[skill] >= restored_skill_cap(skill) ) return;

    // Level bonus for skill improvement.
    amount += amount * random(this_object()->query_level() / 8);

    if( undefinedp(learned[skill]) ) learned[skill] = amount;
    else learned[skill] += amount;

    if( undefinedp(skill_gain[skill]) ) skill_gain[skill] = amount;
    else skill_gain[skill] += amount;

    if( undefinedp(skills[skill]) ) skills[skill] = 0;

    SKILL_D(skill)->skill_improved(this_object(), skill);
    apply_restored_skill_progression(skill);
}

// advance_skill()
//
// This function advances the level of a skill by adding specific amount
// to the skill level.

/* 技能成長屬性
 *
 * 對應表 ATTR_GROWTH：技能代碼 → 屬性代碼。
 * 技能等級每到 5 的倍數擲一次，成功則該屬性（裸值）+1，上限 50。
 *   基礎機率：屬性 1~10 100%、11~15 90%、16~20 80%、21~25 60%、26~30 40%、
 *             31~40 30%、41~45 15%、46~49 10%
 *   技能等級倍率：(50 + 等級/2)%，lv5 約 0.5 倍、lv100 1 倍、lv200 1.5 倍
 *   以上再整體 ×1.2
 * 每個門檻一生只擲一次，擲過的最高門檻記在 attr_growth/<技能>。
 * 已經練過的等級不補擲。只對玩家生效。
 */
#define ATTR_GROWTH_MAX 50

static mapping ATTR_GROWTH = ([
    "unarmed":              "str",
    "force":                "con",
    "dodge":                "dex",
    "parry":                "cps",
    "sneak":                "dex",
    "spells":               "spi",
    "magic":                "spi",
    "backstab":             "cor",
    "killerhood":           "cor",
    "literate":             "int",
    "archaic attainment":   "int",
    "absorption":           "wis",
]);

private int attr_growth_base(int value)
{
    if( value <= 10 ) return 100;
    if( value <= 15 ) return 90;
    if( value <= 20 ) return 80;
    if( value <= 25 ) return 60;
    if( value <= 30 ) return 40;
    if( value <= 40 ) return 30;
    if( value <= 45 ) return 15;
    return 10;
}

private void roll_attr_growth(string skill, int old_level, int new_level)
{
    string attr, cname;
    int last, lv, value;

    if( !userp(this_object()) ) return;
    if( !stringp(attr = ATTR_GROWTH[skill]) ) return;

    last = this_object()->query("attr_growth/" + skill);
    if( last < old_level ) last = old_level;

    for(lv = (last / 5 + 1) * 5; lv <= new_level; lv += 5) {
        this_object()->set("attr_growth/" + skill, lv);
        // 善想禪要 100 級改為固定慧根 +3，這一級不再擲機率（使用者定案，2026-10-19）。
        if( skill == "absorption" && lv == 100 ) continue;
        value = query_attr(attr, 1);
        if( value >= ATTR_GROWTH_MAX ) continue;
        // 機率以萬分之一計：基礎% × (50 + lv/2)% × 1.2
        if( random(10000) >= attr_growth_base(value) * (100 + lv) * 3 / 5 ) continue;
        if( !set_attr(attr, value + 1) ) continue;
        cname = ([ "str": "膂力", "cor": "膽識", "int": "悟性", "spi": "靈性",
                   "cps": "定力", "dex": "機敏", "con": "根骨", "wis": "慧根" ])[attr];
        tell_object(this_object(), HIY "你的" + (cname ? cname : attr) + "提高了！\n" NOR);
    }
}

/* 法術道行（使用者定案，2026-10-16）：可 enable 在法術（magic）上的特殊技能，
 * 每升到第 n 級給 n×10 法術道行。只對玩家生效。 */
private void magic_mastery_growth(string skill, int old_level, int new_level)
{
    object daemon;
    int lv;

    if( !userp(this_object()) || skill == "magic" ) return;
    daemon = SKILL_D(skill);
    if( !objectp(daemon) || !function_exists("valid_enable", daemon) ) return;
    if( !daemon->valid_enable("magic") ) return;
    for(lv = old_level + 1; lv <= new_level; lv++)
        this_object()->gain_score("magic mastery", lv * 10);
}

/* 咒術道行（使用者定案，2026-10-24）：比照法術道行，基本咒術（spells）與能裝在咒術上的
 * 武功（天師道法四系、茅山幻術等）每升到第 sk 級給 sk × 30。一次升多級時每一級都給；
 * 不限職業，只對玩家生效。 */
private void spell_mastery_growth(string skill, int old_level, int new_level)
{
    object daemon;
    int lv;

    if( !userp(this_object()) ) return;
    if( skill != "spells" ) {
        daemon = SKILL_D(skill);
        if( !objectp(daemon) || !function_exists("valid_enable", daemon) ) return;
        if( !daemon->valid_enable("spells") ) return;
    }
    for(lv = old_level + 1; lv <= new_level; lv++)
        this_object()->gain_score("spell mastery", lv * 30);
}

/* 基本技能升級給武術造詣（NEW）
 *
 * 基本兵器技能（含雙手、副手）、徒手、閃躲、招架，每升到第 sk 級給
 * sk × 10 點武術造詣（martial art）。一次升多級時每一級都給。只對玩家生效。
 */
static mapping MARTIAL_ART_BASIC = ([
    "unarmed": 1, "dodge": 1, "parry": 1,
    "blade": 1, "twohanded blade": 1, "secondhand blade": 1,
    "sword": 1, "twohanded sword": 1, "secondhand sword": 1,
    "axe": 1, "twohanded axe": 1, "secondhand axe": 1,
    "pike": 1, "twohanded pike": 1, "secondhand pike": 1,
    "staff": 1, "twohanded staff": 1, "secondhand staff": 1,
    "blunt": 1, "twohanded blunt": 1, "secondhand blunt": 1,
    "dagger": 1, "secondhand dagger": 1, "needle": 1, "secondhand needle": 1,
    "whip": 1,
]);

private void basic_skill_martial_art(string skill, int old_level, int new_level)
{
    int lv, gain;

    if( !userp(this_object()) ) return;
    if( !MARTIAL_ART_BASIC[skill] ) return;

    for(lv = old_level + 1; lv <= new_level; lv++) gain += lv * 10;
    if( gain > 0 ) this_object()->gain_score("martial art", gain);
}

varargs void advance_skill(string skill, int amount)
{
    int old_level;

    if( !amount ) amount = 1;
    old_level = skills[skill];

    if( undefinedp(skills[skill]) )
	skills[skill] = amount;
    else
	skills[skill] += amount;

    if( skills[skill] > 200 ) skills[skill] = 200;

    SKILL_D(skill)->skill_advanced(this_object(), skill);
    magic_mastery_growth(skill, old_level, skills[skill]);
    spell_mastery_growth(skill, old_level, skills[skill]);
    roll_attr_growth(skill, old_level, skills[skill]);
    basic_skill_martial_art(skill, old_level, skills[skill]);

    if( skills[skill] > skills[best_skill] ) best_skill = skill;
}

// abandon_skill()
//
// This function toggles abandon flag of specific skill.

varargs void abandon_skill(string skill, int restore)
{
    if( undefinedp(learned[skill]) ) return;

    if( restore ) skill_flag[skill] &= (~SKILL_FLAG_ABANDONED);
    else	  skill_flag[skill] |= SKILL_FLAG_ABANDONED;

    if( !skill_flag[skill] ) map_delete(skill_flag, skill);
}

int query_skill_map_num()
{
    mapping my_skill_map;
    mixed *levels;

    my_skill_map = this_object()->query_skill_map();
    if( !mapp(my_skill_map) ) return 0;
    levels = values(my_skill_map);
    levels -= ({ 0 });
    return sizeof(levels);
}

