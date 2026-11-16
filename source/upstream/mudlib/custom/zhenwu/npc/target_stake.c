#include <ansi.h>
inherit "/feature/npc/dummy";

void create()
{
    set_name("假人", ({"target stake","stake","假人"}));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("long", "這是一具軍營內供新兵練武的粗麻木樁，樁身佈滿刀痕與箭孔。它被牢牢固定在地面，不會還手，也不會因操練而損壞，正適合反覆磨練基本招式。\n");
    setup();
}
