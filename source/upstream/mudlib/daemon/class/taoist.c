#include <ansi.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string query_rank(object ob, string politeness)
{
    if( !politeness ) return "道士";
    switch(politeness) {
    case "self": return "貧道";
    case "respectful": return "道長";
    default: return "道士";
    }
}

// Race 基數 as a percentage (100 = human baseline), read from the race
// daemon's "commoner_score_base"; only scales character level-up thresholds.
private int race_coef(object ob)
{
    int coef;
    coef = RACE_D(ob->query_race())->query("commoner_score_base");
    return coef > 0 ? coef : 100;
}

// cur_lv is the level being left; thresholds target cur_lv + 1.
void set_next_target(object ob, int cur_lv)
{
    int lv, n, coef;

    if( cur_lv < 1 ) cur_lv = 1;
    lv = cur_lv + 1;
    n = lv - 1;
    coef = race_coef(ob);

    ob->set_target_score("survive", n*n*100 * coef / 100);
    ob->set_target_score("magic", n*n*150 * coef / 100);
    ob->set_target_score("spell mastery",
        lv > 10 ? (lv-10)*(lv-10)*100 * coef / 100 : 0);
    ob->set_target_score("combat",
        lv > 11 ? (lv-11)*(lv-11)*100 * coef / 100 : 0);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    int gin_gain, kee_gain, sen_gain;

    gin_gain = ob->query_attr("dex", 1) / 4 + random(3) - 1;
    kee_gain = ob->query_attr("con", 1) / 4 + random(3) - 1;
    sen_gain = ob->query_attr("spi", 1) / 3 + random(3) - 1;

    ob->set_stat_maximum("gin", ob->query_stat_maximum("gin") + gin_gain);
    ob->set_stat_maximum("kee", ob->query_stat_maximum("kee") + kee_gain);
    ob->set_stat_maximum("sen", ob->query_stat_maximum("sen") + sen_gain);

    tell_object(ob, HIY "你對天地氣機的體悟更深，精氣神隨之增長。\n" NOR);
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}