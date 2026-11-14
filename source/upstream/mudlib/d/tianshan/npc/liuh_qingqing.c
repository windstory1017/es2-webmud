// /d/tianshan/npc/liuh_qingqing.c — 天山 NPC：柳清清（X2 小木屋）。
//
// 玩家走進小木屋，她說最近要祭奠爺爺；玩家把供品（id gong，風城小販賣）交給她，她就把人送進內堂（靈堂）。

#include <npc.h>

inherit F_VILLAGER;

#define HALL  "/d/tianshan/hall"
#define TICK  2   // 1 tick 約 2 秒

void create()
{
    seteuid(getuid());
    set_name("柳清清", ({ "liuh qingqing", "liuh", "qingqing" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "female");
    set("age", 18);
    set("long", "柳家的後人，跟柳東蘆是遠親，一身素淨的布衣，眉眼間帶著幾分書卷氣。屋裡屋外打理得一塵不染，看得出對家裡的先人十分敬重。\n");
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
}

// 玩家走進來，過 1 tick 說一句。
void init()
{
    object who = this_player();

    ::init();
    if( objectp(who) && userp(who) ) call_out("greet", TICK, who);
}

void greet(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say 你好啊，我最近要祭奠我爺爺了。");
}

// 收下供品：只收供品，收下後送進內堂。
int accept_object(object who, object ob)
{
    if( !ob->id("gong") ) {
        command("say 這東西我用不上，多謝你的好意。");
        return 0;
    }
    call_out("send_in", 0, who, ob);
    return 1;
}

void send_in(object who, object ob)
{
    if( objectp(ob) ) destruct(ob);
    if( !objectp(who) || environment(who) != environment() ) return;
    message_vision("$N點了點頭，將$n送進內堂。\n", this_object(), who);
    if( who->move(HALL) )
        message("vision", replace_string("$N被柳清清領了進來。", "$N", who->name()) + "\n", environment(who), who);
}
