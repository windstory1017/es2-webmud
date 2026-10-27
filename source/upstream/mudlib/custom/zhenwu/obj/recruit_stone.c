// recruit_stone.c -- 徵兵石, an immovable prop at 魯熙年's post that lets a
// qualifying commoner or fighter "enlist" to become a soldier. Once a
// character has left "commoner" for any class, that choice is permanent
// (see the origin-class check below) -- so this is also the one place that
// currently needs to enforce "no re-converting" at all, since no other
// class-conversion path exists in the mudlib yet.
#include <ansi.h>

inherit ITEM;

void create()
{
    set_name("徵兵石", ({"recruit stone", "recruitstone", "recruit", "stone"}));
    set("no_get", "徵兵石深植地下，任你怎麼使力也搬不動它。\n");
    set("long", "振武軍營徵兵處，可在此登記 (enlist) 成為軍人。\n");
    set_weight(999999);
    setup();
}

void init()
{
    add_action("do_enlist", "enlist");
}

int do_enlist(string arg)
{
    object me, who;
    string origin_class;

    me = this_player();
    if( !me || environment(me) != environment(this_object()) ) return 0;

    origin_class = me->query_class();
    if( origin_class != "commoner" && origin_class != "fighter" )
        return notify_fail("你已經有自己的身分了，振武軍營不收你這樣的人。\n");

    if( me->query("gender") != "male" )
        return notify_fail("振武軍營目前只招募男丁入伍。\n");

    me->set_class("soldier");
    me->set("title", "振武軍營軍人");
    me->set("custom_faction", "soldier.ber");   // 振武軍營門派記號（使用者指示，2026-10-22）
    // 武者轉任的軍人：可以當軍人，但學不到軍人的特殊技能（使用者定案，2026-10-22）。
    if( origin_class == "fighter" ) me->set("soldier/from_fighter", 1);

    // Only a fresh commoner recruit gets the signature soldier skills. A
    // fighter who enlists already has their own combat background, so they
    // become a soldier without ever picking these up -- there is currently
    // no other way to learn them in the mudlib, so simply not granting them
    // here is enough to keep that permanent.
    if( origin_class == "commoner" ) {
        me->set_skill("berserk", 1);
        me->set_learn("berserk", 1);
        me->set_skill("powerblow", 1);
        me->set_learn("powerblow", 1);
        // 強力格擋改由米沛傳授（軍人、等級與戰場功勳達標時觸發劇情）。
    }

    // The rest of the welcome (魯熙年's dialogue, then the final
    // confirmation) plays out over several ticks -- see
    // lu_xinien.c:welcome_recruit().
    if( objectp(who = present("xinien", environment(this_object()))) )
        who->welcome_recruit(me);

    return 1;
}
