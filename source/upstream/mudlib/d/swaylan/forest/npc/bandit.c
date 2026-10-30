// /d/swaylan/forest/npc/bandit.c — 密林 NPC：盜賊（依水嵐縣設計表第五批，docs/AREA_SWAYLAN.md）。

#include <npc.h>
inherit F_FIGHTER;

// 看到玩家就下殺手（照老松林土匪）。
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
    set_name("盜賊", ({ "bandit" }));
    set_race("human");
    set_class("thief");
    set_level(10);
    set("age", 25);
    set("long", @LONG
一個滿臉橫肉的盜賊，身穿粗布衣，手裡提著一柄長劍，眼神兇狠，見了生人便撲上來，下手毫不留情。
LONG
    );
    set_skill("sword", 20);
    set_skill("dodge", 20);
    set_skill("parry", 20);
    set_skill("unarmed", 20);
    set("bounty", ([ "military service": 45 ]));   // 擊殺者獲得戰場功勳
    set_power("B");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();
    carry_object("/d/oldpine/obj/coarse_cloth")->wear();
    carry_money("silver", 5 + random(26));   // 隨機 5～30 兩
}
