// /d/ziyan/npc/constable.c — 紫煙鎮 NPC：捕快（依水嵐縣設計表）。牢房各兩名；內廳縣令喊人時也會叫出兩名。

#include <npc.h>
inherit F_SOLDIER;

void create()
{
    seteuid(getuid());
    set_name("捕快", ({ "constable" }));
    set_race("human");
    set_class("soldier");
    set_level(30);
    set("age", 25);
    set("long", @LONG
一名腰挎單刀的捕快，身穿皂衣，目光在人群中來回打量。武藝雖然平平，但那身官服往身上一套，尋常百姓見了也不敢輕舉妄動。
LONG
    );
    set_skill("unarmed", 60);
    set_skill("dodge", 60);
    set_skill("parry", 60);
    set_skill("blade", 60);
    set_power("A");
    setup();
    carry_object("/obj/area/obj/blade")->wield();
}
