// /d/baihua/npc/nan_kong_hen.c — 百花村 NPC：「落山風」南宮恨（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("南宮恨", ({ "nan kong hen", "nan", "hen" }));
    set_race("human");
    set_class("taoist");
    set_level(40);
    set("gender", "male");
    set("age", 45);
    set("long", @LONG
南宮恨一臉秀氣, 身著白淨衣裳, 徐風吹來身形搖蕩, 似乎有些瘦弱. 平時不問人間俗事, 山林為屋, 草木為衣, 枕流漱石修身養性, 故以中年之齡在道術上已有相當領悟.
LONG
    );
    set("nickname", "落山風");
    set("custom_faction", "taoist.storm");   // 玄衣派（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
}
