// /d/swaylan/buxuan/npc/farmer.c — 步玄派 NPC：農夫（照李家村農夫，敘述改成步玄派的）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("農夫", ({ "farmer" }));
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("age", 25);
    set("long", @LONG
步玄派裡負責耕作的農夫，皮膚曬得黝黑，手掌粗糙，閒暇時也會捧著一卷書，坐在田埂上讀上幾頁。
LONG
    );
    set_power("C");
    setup();
}
