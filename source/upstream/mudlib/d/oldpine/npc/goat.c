// /d/oldpine/npc/goat.c — 老松林草叢、百花村山腰上的野獸：山羊（使用者指定，2026-10-24）。
// 野獸強度 2，不主動攻擊。

#include <npc.h>

void create()
{
    seteuid(getuid());
    set_name("山羊", ({ "goat" }));
    set_race("beast");
    set_class("commoner");
    set_level(1);
    set("long", @LONG
在山壁上到處爬來爬去，低頭吃著為數不多的野草。
LONG
    );
    set("unit", "隻");
    set("beast_actions", ({
        ([ "action": "$N低下頭，用彎角往$n的$l撞去", "damage_type": "撞傷" ]),
        ([ "action": "$N後腿一蹬，往$n的$l踢去", "damage_type": "瘀傷" ]),
        ([ "action": "$N咩地叫了一聲，張口往$n的$l咬去", "damage_type": "咬傷" ])
    }));
    // 野獸強度（1～10），數值見 daemon/race/beast.c。
    set_beast(2);
    setup();
}
