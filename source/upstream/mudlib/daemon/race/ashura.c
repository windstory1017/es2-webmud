/* CUSTOM RACE: 阿修羅 (ashura). Original race data 2006/09/15. */
#define BASE_WEIGHT 40000
#include <race.h>
inherit HUMANOID;

private void create() {
    ::create(); seteuid(getuid());
    set("karma", 40); set("civilized", 1); set("commoner_score_base", 180);
    set("class_level_cap", ([
        "commoner":1, "fighter":70, "taoist":60, "alchemist":50,
        "soldier":70, "scholar":-1, "monk":-1, "thief":50
    ]));
    DAEMON_D->register_race_daemon("ashura");
}
void setup(object ob) {
    ::setup(ob); ob->set_default_object(__FILE__);
    if (!ob->query_weight()) ob->set_weight(BASE_WEIGHT + ((int)ob->query_attr("str",1)-13)*5000);
    ob->add_temp("apply/attack", 40);
    ob->add_temp("apply/intimidate", 30);
    ob->add_temp("apply/spells", 15);          // 咒術 +15（技能等級）
    ob->set_temp("apply/vision_of_ghost", 1);  // 陰陽眼
}
void initialize(object ob) {
    ::initialize(ob);
    ob->init_attribute(([
        "str":20+random(6), "cor":25+random(16), "int":13+random(6), "spi":14+random(9),
        "cps":5+random(6), "dex":15+random(6), "con":17+random(6), "wis":13+random(6)
    ]));
    ob->init_statistic(([ "gin":80, "kee":80, "sen":80 ]));
    if (!ob->query("age")) ob->set("age", 14+random(4));
    ob->set_default_object(__FILE__);
}
