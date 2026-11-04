// /d/graveyard/npc/dirty_taoist.c — 亂葬崗 NPC：邋遢道士（Dirty taoist），常在祠堂喝酒（依亂葬崗設計表）。
inherit "/custom/legacy_npc/legacy_npc";

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
    set_power("C");
    setup();
    carry_object("/custom/armor/cloth/purple_taoist_robe")->wear();
    carry_object("/d/graveyard/obj/chrysoberyl_ring")->wear();
    carry_object("/d/graveyard/obj/urn");
}
