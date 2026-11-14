// /d/mengjia/npc/white_tiger.c — 孟家莊 野獸：白額虎（F10 密林，強度 8）。看到玩家就下殺手；
// 沒打死前，往北進孟家祖墳的路被擋住（判斷在 forest.c）。

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
    set_name("白額虎", ({ "white tiger", "tiger" }));
    set_race("beast");
    set_class("commoner");
    set_level(1);
    set("white_tiger", 1);
    set("long", "一頭額上生著白毛的猛虎，體型比尋常老虎大上一圈，口鼻間還沾著未乾的血，一雙虎目死死盯著你。\n");
    set("unit", "頭");
    set("beast_actions", ({
        ([ "action": "$N低吼一聲猛撲上來，張開血盆大口往$n的$l咬去", "damage_type": "咬傷" ]),
        ([ "action": "$N人立而起，兩隻前爪往$n的$l狠狠拍下", "damage_type": "抓傷" ]),
        ([ "action": "$N尾巴一甩，虎爪貼地橫掃，直抓$n的$l", "damage_type": "抓傷" ])
    }));
    // 野獸強度（1～10），數值見 daemon/race/beast.c。
    set_beast(8);
    setup();
}
