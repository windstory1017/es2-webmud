#include <ansi.h>
// /d/wutang/npc/town_god.c — 五堂鎮 NPC：城隍爺（依五堂鎮設計表）。

inherit "/custom/ghost/std_ghost";

// 鬼魂向城隍爺報到（register）就能還陽復活，規則同雪亭鎮小廟的紅色漩渦。
void init()
{
    ::init();
    add_action("do_register", "register");
}

int do_register(string arg)
{
    object me = this_player();

    if( me->query("life_form") != "ghost" )
        return notify_fail("你陽壽未盡，用不著向城隍爺報到。\n");
    message_vision(CYN + "$N向城隍爺跪拜報到，城隍爺翻了翻生死簿，點了點頭。\n" NOR, me);
    write("城隍爺說道：「你陽壽未盡，回去吧。」\n你覺得一陣暈眩 ...\n");
    CHAR_D->make_living(me);
    me->start_busy(3);
    return 1;
}

void create()
{
    set_name("城隍爺", ({ "town god", "god" }));
    set("age", 800);
    set("long", @LONG
你面前這位身穿紫紅色官服的老人便是此地的城隍爺了﹐土地公掌管陽間之事﹐城隍爺掌管的就是陰間之事﹐陽間的人死了﹐鬼魂都要找城隍爺報到(register)才能往生輪迴﹐否則便會淪落為孤魂野鬼。
LONG
    );
    set("ghost_chat", ({ }));
    setup_ghost(500, 500);
    // 城隍爺坐鎮廟中，不四處遊蕩。
    delete("chat_chance");
    delete("chat_msg");
}
