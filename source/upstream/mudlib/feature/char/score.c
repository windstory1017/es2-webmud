#include <ansi.h>
#include <dbase.h>
#include <login.h>

string query_class() { return query("class"); }
string query_race() { return query("race"); }
int query_level() {    return query("level"); }

int set_class(string new_class)
{
    int lvl;

    /* CUSTOM A-H RACE RULE: class_level_cap < 0 means this race cannot join
     * the class at all (原始種族資料的「無」). */
    if( new_class && query_race()
    &&  RACE_D(query_race())->query("class_level_cap/" + new_class) < 0 )
	return 0;

    set("class", new_class);
    if( query_level() && query_race() ) {
	delete("target_score");
	RACE_D(query_race())->initialize(this_object());
	CLASS_D(new_class)->initialize(this_object());
    }
    return 1;
}

void set_race(string new_race)
{
    int lvl;

    set("race", new_race);
    if( query_level() && query_class() ) {
	delete("target_score");
	RACE_D(new_race)->initialize(this_object());
	CLASS_D(query_class())->initialize(this_object());;
    }
}

void set_level(int lvl)
{
    string s;

    if( lvl < 1 ) error("character level must be at least 1.\n");
    if( lvl > 999 ) error("character level must less than 999.\n");
    set("level", lvl);
    if( query_race() && query_class() ) {
	delete("target_score");
	RACE_D(query_race())->initialize(this_object());
	CLASS_D(query_class())->initialize(this_object());
    }
}

int query_score(string course)
{
    return query("score/" + course);
}

static mapping score_gain = ([]);
mapping query_score_gain() { return score_gain; }
void reset_score_gain() { score_gain = ([]); }

varargs void
gain_score(string course, int xp)
{
    add("score/" + course, xp);
    if( undefinedp(score_gain[course]) ) score_gain[course] = xp;
    else score_gain[course] += xp;
    /* 經驗只累積；人物等級要在使用 gain 時才會提升，見 try_level_up()。 */
}

/* CUSTOM A-H RACE RULE: per-race class level cap from race daemon.
 * 已達種族的職業等級上限時傳回 1（不能再升級）。
 * 平民不分種族一律上限 50（使用者定案，2026-10-20）。 */
#define COMMONER_LEVEL_CAP  50

int at_level_cap()
{
    int cap;

    if( query_class() == "commoner" )
        cap = COMMONER_LEVEL_CAP;
    else
        cap = RACE_D(query_race())->query("class_level_cap/" + query_class());
    return cap != 0 && query_level() >= cap;
}

/* gain 指令呼叫：各項經驗都達到升級所需時，提升一級（每次 gain 最多一級）。
 * 傳回 1 表示有升級。 */
int try_level_up()
{
    mapping sc, targ_sc;
    string s, save_file;
    int v;

    sc = query("score");
    targ_sc = query("target_score");
    if( !mapp(sc) || !mapp(targ_sc) ) return 0;

    foreach(s, v in targ_sc)
	if( sc[s] < v ) return 0;

    if( at_level_cap() ) return 0;   // 已達上限，不提示

    receive( HIY "你的等級提昇了﹗\n" NOR );
    RACE_D(query_race())->advance_level(this_object());
    CLASS_D(query_class())->advance_level(this_object());
    delete("praise_done");
    add("level", 1);

    seteuid(getuid());
// 系統強制在玩家升級時做save及backup -Dragoon
#ifdef SAVE_USER
    this_object()->save();
    save_file = this_object()->query_save_file();
    cp(save_file, save_file+".backup");
#endif
    receive("檔案儲存及備份完畢。\n");
    return 1;
}

int query_target_score(string course)
{
    return query("target_score/" + course);
}

void set_target_score(string course, int xp)
{
    if( !query("score/" + course) ) set("score/" + course, 0);
    if( xp < (int)query("target_score/" + course) ) return;
    set("target_score/" + course, xp);
}


