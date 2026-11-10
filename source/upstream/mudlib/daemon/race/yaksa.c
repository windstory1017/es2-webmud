/* CUSTOM RACE: 夜叉 (yaksa). Original race data 2006/09/15. */
#define BASE_WEIGHT 40000
#include <race.h>
inherit HUMANOID;

private void create() {
    ::create(); seteuid(getuid());
    set("karma", 50); set("civilized", 1); set("commoner_score_base", 140);
    set("class_level_cap", ([
        "commoner":1, "fighter":65, "taoist":65, "alchemist":60,
        "soldier":60, "scholar":-1, "monk":-1, "thief":70
    ]));
    DAEMON_D->register_race_daemon("yaksa");
}
void setup(object ob) {
    ::setup(ob); ob->set_default_object(__FILE__);
    if (!ob->query_weight()) ob->set_weight(BASE_WEIGHT + ((int)ob->query_attr("str",1)-13)*5000);
    ob->add_temp("apply/armor", 15);
    ob->add_temp("apply/attack", 10);
    ob->set_temp("apply/vision_of_ghost", 1);  // 陰陽眼
}
void initialize(object ob) {
    ::initialize(ob);
    ob->init_attribute(([
        "str":16+random(10), "cor":17+random(9), "int":13+random(6), "spi":13+random(6),
        "cps":13+random(6), "dex":13+random(6), "con":16+random(10), "wis":13+random(6)
    ]));
    ob->init_statistic(([ "gin":140, "kee":70, "sen":70 ]));
    if (!ob->query("age")) ob->set("age", 14+random(4));
    ob->set_default_object(__FILE__);
}
