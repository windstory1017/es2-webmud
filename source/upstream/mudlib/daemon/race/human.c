/*---
description: 人類種族的定義。
author: Annihilator <taedlar@gmail.com>
custom: WebMUD A-H race rebalance based on user-supplied 種族設定表.
---*/
#define BASE_WEIGHT 40000

#include <ansi.h>
#include <statistic.h>
#include <race.h>

inherit HUMANOID;

private void create() {
    ::create();

    seteuid(getuid());
    set("karma", 5);
    set("civilized", 1);
    set("commoner_score_base", 100);
    set("class_level_cap", ([
        "commoner":1, "fighter":50, "taoist":50, "alchemist":55,
        "soldier":50, "scholar":60, "monk":65, "thief":50
    ]));

    DAEMON_D->register_race_daemon("human");
}

void setup(object ob) {
    ::setup(ob);

    ob->set_default_object(__FILE__);
    if (!ob->query_weight())
        ob->set_weight(BASE_WEIGHT + ((int)ob->query_attr("str", 1) - 13) * 5000);

    ob->add_temp("apply/armor", 10);
}

void initialize(object ob) {
    ::initialize(ob);

    // 人類族的機率改由選種族時判定（1%，見 LOGIN_D get_race()，使用者定案 2026-10-22）。

    ob->init_attribute(([
        "str":13 + random(6), "cor":13 + random(6),
        "int":13 + random(6), "spi":13 + random(6),
        "cps":13 + random(6), "dex":13 + random(6),
        "con":13 + random(6), "wis":13 + random(6)
    ]));

    ob->init_statistic(([ "gin":35, "kee":35, "sen":35 ]));

    if (!ob->query("age")) ob->set("age", 14 + random(4));
    ob->set_default_object(__FILE__);
}
