// /d/swaylan/temple/npc/abbot.c — 寶蓮寺 NPC：住持 無想禪師（依水嵐縣設計表）。
// 販售四本經書；迷羅香等迷羅香法做好再上架。

#include <npc.h>
inherit F_FIGHTER;
inherit F_VENDOR;

void init()
{
    ::init();
    add_action("do_vendor_list", "list");
}

void create()
{
    seteuid(getuid());
    set_name("無想禪師", ({ "abbot", "wu xiang" }));
    set("title", "寶蓮寺住持");
    set_race("human");
    set_class("monk");
    set_level(65);
    set("gender", "female");
    set("age", 65);
    set("long", @LONG
一位年約六旬的老尼，面容慈祥莊嚴，一雙眼睛溫和明淨，彷彿能看進人的心底。她在寶蓮寺修行多年，一心向佛，說起話來輕聲細語，卻自有一股教人肅然起敬的氣度。你可以用 list 看寺裡結緣的經書，用 buy 請購。
LONG
    );
    set_skill("unarmed", 130);
    set_skill("dodge", 130);
    set_skill("parry", 130);
    set_skill("staff", 130);
    // merchandise 的數字是庫存份數，價錢是各物品自己的 value（四本經書各五兩銀子）。
    set("merchandise", ([
        "/obj/books/book_of_almsdeed_story" : 20,   // 【法華經】
        "/obj/books/book_of_mercy" : 20,            // 【大悲咒】
        "/obj/books/book_of_afterlife" : 20,        // 【往生經】
        "/obj/books/book_of_buddha" : 20,           // 【佛名經】
    ]));
    set_power("S");
    setup();
    carry_object("/custom/weapon/staff/zenstaff")->wield();
    carry_object("/d/swaylan/temple/obj/dabei_robe")->wear();
    carry_object("/d/swaylan/temple/obj/rosary")->wear();
}
