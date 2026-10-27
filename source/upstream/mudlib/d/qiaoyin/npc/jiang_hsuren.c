// /d/qiaoyin/npc/jiang_hsuren.c — 喬陰縣 NPC：江述仁（依使用者設計表，2026-10-16）。

#include <ansi.h>
#include <npc.h>
inherit F_VILLAGER;
inherit "/obj/alchemist/pill_vendor";   // 二階丹藥三倍價（使用者指示 #6，2026-10-23）

void create()
{
    seteuid(getuid());
    set_name("江述仁", ({ "jiang hsuren", "jiang", "hsuren" }));
    set("title", "龍圖丹派大弟子");
    set_race("human");
    set_class("alchemist");
    set_level(20);
    set("gender", "male");
    set("age", 32);
    set("long", @LONG
龍圖丹派大弟子，對丹道頗有天份，小有成就。但個性輕浮、不務正業，最近似乎捅了甚麼婁子，異常安份。
LONG
    );
    // 販售丹藥（使用者指示，2026-10-22）。
    set("merchandise", ([
        "/obj/alchemist/minor_heal_pill" : 10,           // 小還丹
        "/obj/alchemist/mana_pill" : 10,                 // 醒神丹
        "/obj/alchemist/aquapill" : 50,                  // 金液丹
        "/obj/alchemist/refresh_pill" : 10,              // 金鎖固精丸
        "/obj/alchemist/lavage_pill" : 5,              // 清腹丹
        "/obj/alchemist/relax_pill" : 5,               // 養心丹
        // 以上丹藥數量依使用者指示 #5（2026-10-23）
        "/obj/alchemist/heat_firewood" : 3000,           // 松柴
        "/obj/alchemist/heat_reactor" : 30,              // 青銅丹鼎
        "/obj/alchemist/heat_furnace" : 5,               // 青銅丹爐
        // 煉丹用具依使用者指示 #4（2026-10-23）
    ]));
    // 江述仁修改紀錄 #4（2026-10-22 執行；docs/martial_arts/龍圖丹派_江述仁修改紀錄.md）：傳授這三項技能。
    set_skill("alchemy-medication", 25);
    set_skill("alchemy-wealth", 35);
    set_skill("zushang", 50);
    set_power("A");
    setup();
    carry_object("/obj/cloth")->wear();
}

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

#define ALCHEMY_FEE 50000   // 柴火錢：五萬文（五兩黃金；等值貨幣、超過都可以）

// 煉丹房的柴火錢：方士給江述仁五萬文以上，本次登入期間可以進出北邊的煉丹房，
// 也可以跟江述仁學神農之術、黃白之術、竹上經心法。
int accept_object(object who, object ob)
{
    if( !ob->query("money_id") ) return 0;
    if( who->query_class() != "alchemist" ) {
        command("say 這丹房只借給方士用，閣下的錢還請收回。");
        return 0;
    }
    if( who->query_temp("qiaoyin/alchemy_paid") ) {
        command("say 你方才已經付過了，進去吧。");
        return 0;
    }
    if( ob->value() < ALCHEMY_FEE ) {
        command("say 就這麼一點？連柴火都買不起。");
        return 0;
    }
    who->set_temp("qiaoyin/alchemy_paid", 1);
    command("say 嘿嘿，好說好說，丹房就在後頭，請便；如果還不會丹道之術的話，也可以跟我學習。");
    return 1;
}

// 江述仁修改紀錄 #5、#6（2026-10-22 執行；docs/martial_arts/龍圖丹派_江述仁修改紀錄.md）：付過柴火錢的玩家可以學這三項技能，
// 每項各自最多學到經驗值 50（不是等級 50）。
int acquire_skill(object me, string skill)
{
    if( member_array(skill, ({ "alchemy-medication", "alchemy-wealth", "zushang" })) < 0 )
        return 0;
    if( !me->query_temp("qiaoyin/alchemy_paid") ) {
        command("say 想學？先把柴火錢付了再說。");
        return 1;
    }
    if( me->query_learn(skill) >= 50 ) {
        command("say 我會的都教你了，剩下的自己練去。");
        return 1;
    }
    message_vision(CYN + "$N將「" + to_chinese(skill) + "」的入門要訣傳授給$n。\n" NOR,
        this_object(), me);
    me->improve_skill_exact(skill, 50 - me->query_learn(skill));
    if( me->query_learn(skill) > 50 ) me->set_learn(skill, 50);
    return 1;
}
