#include <ansi.h>


#include <npc.h>

inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("青衣漢子", ({"guard"}));
    set_race("human");
    set_class("fighter");
    set_level(10);

    set_skill("blade", 50);

    set("age", 26);
    set("long", "一個身穿藏青勁裝的漢子﹐雙目炯炯有神地盯著你。\n");
    set_power("C");
    setup();
    carry_object( STOCK_ARMOR("cloth") )->wear();
    carry_object( STOCK_WEAPON("longsword") )->wield();
    carry_money("coin", 160);
}


// 不再攔人（使用者指示，2026-11-06）：雪吟莊大門、後花園也用這個檔案。
// 交出英雄帖仍會領人過橋進入雪吟莊（雪亭鎮白衣女子的任務線）。

void greet_visitor(object ob, object invitation)
{
    destruct(invitation);
    if( environment(ob)!=environment() ) return;

    message_vision(CYN + name() + "引領$N過了橋﹐進入雪吟莊。\n" NOR, ob);
    ob->move("/d/snowkeep/entrance");
}

int accept_object(object who, object ob)
{
    if( !ob->id("invitation") ) return 0;

    command("say 小人失敬﹗敝莊主已經恭候多時了。\n");
    do_chat((: greet_visitor, who, ob :));
    return 1;
}

