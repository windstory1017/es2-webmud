// /d/graveyard/npc/old_bonze.c — 亂葬崗 NPC：老和尚（Old bonze），在石亭旁替無主孤魂誦經（依亂葬崗設計表）。
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("老和尚", ({ "old bonze", "bonze" }));
    set_race("human");
    set_class("monk");
    set_level(20);
    set("gender", "male");
    set("age", 80);
    set("long", @LONG
一位鬚眉皆白的老和尚，身上的灰布僧袍洗得發白，手中捻著一串念珠，口中低聲誦著往生咒。他神情慈和，步履卻沉穩得很，那根沉甸甸的禪杖在他手裏，竟似輕若無物。
LONG
    );
    set_power("B");
    setup();
    carry_object("/d/graveyard/obj/brass_zenstaff")->wield();
    carry_object("/d/graveyard/obj/rosary")->wear();
}
