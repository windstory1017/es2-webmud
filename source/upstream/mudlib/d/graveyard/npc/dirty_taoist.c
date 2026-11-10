// /d/graveyard/npc/dirty_taoist.c — 亂葬崗 NPC：邋遢道士（Dirty taoist），常在祠堂喝酒（依亂葬崗設計表）。
//
// 風嘯拳法任務（使用者指示，2026-11-05，設計見 docs/martial_arts/共通_風嘯拳法.md「任務」）：
// 道士會自己喝酒，喝到第 DRUNK_SIPS 口就醉倒，醉倒 DRUNK_TICKS 個 tick 後醒來。
// 醉倒時玩家可以從他身上搜出貓眼戒指與骨灰罈（get all from taoist），也可以直接殺了他拾取。
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define TICK        2                   // 一個 tick 的秒數
#define DRUNK_SIPS  3                   // 喝幾口醉倒
#define DRUNK_TICKS 30                  // 醉倒幾個 tick

int sips;

void drink();

void create()
{
    seteuid(getuid());
    set_name("邋遢道士", ({ "dirty taoist", "taoist" }));
    set_race("human");
    set_class("taoist");
    set_level(10);
    set("gender", "male");
    set("age", 35);
    set("long", @LONG
一個蓬頭垢面的道士，一身紫色道袍又皺又髒，前襟沾滿了酒漬，渾身酒氣薰人。他倚著供桌醉眼惺忪，嘴裏不知在嘟囔些什麼，偶爾抬眼看人，目光卻清醒得出奇。
LONG
    );
    // 約 5～7 tick 一次，大多是喝酒。
    set("chat_chance", 15);
    set("chat_msg", ({
        (: drink :),
        (: drink :),
        "邋遢道士對著牌位嘟囔了幾句，誰也聽不清他在說些什麼。\n",
    }));
    set_power("C");
    setup();
    carry_object("/custom/armor/cloth/purple_taoist_robe")->wear();
    carry_object("/d/graveyard/obj/chrysoberyl_ring")->wear();
    carry_object("/d/graveyard/obj/urn");
}

void drink()
{
    if( !living(this_object()) || is_fighting() ) return;
    if( ++sips < DRUNK_SIPS ) {
        message_vision(CYN "$N摸出一個酒葫蘆﹐仰頭咕嘟咕嘟灌了幾口﹐打了個酒嗝。\n" NOR, this_object());
        return;
    }
    sips = 0;
    message_vision(CYN "$N又灌了一大口酒﹐眼皮越來越沉﹐身子晃了兩晃。\n" NOR, this_object());
    unconcious();
    // 醉倒的時間比一般昏迷長，玩家才來得及搜身。
    remove_call_out("revive");
    call_out("revive", DRUNK_TICKS * TICK);
}
