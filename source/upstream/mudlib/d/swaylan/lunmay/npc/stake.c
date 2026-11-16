// /d/swaylan/lunmay/npc/stake.c — 冷梅莊 NPC：假人（照振武軍營的假人，敘述改成冷梅莊的）。

#include <ansi.h>
inherit "/feature/npc/dummy";

void create()
{
    set_name("假人", ({"target stake","stake","假人"}));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("long", "一具用粗木紮成的假人，牢牢釘在地上，身上佈滿了深淺不一的劍痕，是冷梅莊弟子平日練劍用的。\n");
    setup();
}
