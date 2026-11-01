// /d/swaylan/obj/fishing_stick.c — 釣竿（五陵客棧店小二販售）。釣魚功能之後再做，目前只是道具。

inherit ITEM;

void create()
{
    set_name("釣竿", ({ "fishing stick", "stick" }));
    set_weight(800);
    if( !clonep() ) {
        set("unit", "根");
        set("value", 300);
        set("long", "一根細長的竹製釣竿，竿身打磨得光滑，末端繫著一截絲線與小小的鐵鉤。\n");
    }
    setup();
}
