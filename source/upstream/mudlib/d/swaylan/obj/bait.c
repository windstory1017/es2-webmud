// /d/swaylan/obj/bait.c — 魚餌（五陵客棧店小二販售）。設計表：先給一段敘述就好。

inherit COMBINED_ITEM;

void create()
{
    set_name("魚餌", ({ "bait" }));
    if( !clonep() ) {
        set("unit", "包");
        set("base_unit", "團");
        set("base_value", 1);
        set("base_weight", 10);
        set("long", "用麥麩、蚯蚓和酒糟搓成的小團魚餌，聞起來有股說不出的腥香，想來魚兒會喜歡。\n");
    }
    setup();
}
