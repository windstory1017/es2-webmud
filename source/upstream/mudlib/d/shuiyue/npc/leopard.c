// /d/shuiyue/npc/leopard.c — 水月村 野獸：炎豹（強度 4），看到玩家就下殺手。

#include <npc.h>

void init()
{
    object ob = this_player();

    ::init();
    if( objectp(ob) && userp(ob) && living(this_object()) && !is_fighting(ob)
    &&  !environment()->query("no_fight") && visible(ob) ) {
        kill_ob(ob);
    }
}

void create()
{
    seteuid(getuid());
    set_name("炎豹", ({ "leopard" }));
    set_race("beast");
    set_class("commoner");
    set_level(1);
    set("long", "一頭身上帶著火焰般橙紅斑紋的猛豹，兩眼在暗處閃著冷光，正無聲地盯著你。\n");
    set("unit", "頭");
    set("beast_actions", ({
        ([ "action": "$N猛地撲上來，張口往$n的$l咬去", "damage_type": "咬傷" ]),
        ([ "action": "$N伸出利爪往$n的$l抓去", "damage_type": "抓傷" ]),
        ([ "action": "$N低吼一聲，縱身躍起，爪子直取$n的$l", "damage_type": "抓傷" ])
    }));
    // 野獸強度（1～10），數值見 daemon/race/beast.c。
    set_beast(4);
    setup();
}
