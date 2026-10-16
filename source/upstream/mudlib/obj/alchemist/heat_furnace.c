#include "/daemon/misc/heat_b.h"
inherit ITEM;
private int burning, fuel_phase, ash_phase;
int is_heat_furnace() { return 1; }
int query_burning() { return burning; }
void set_burning(int n) { burning = n; }
int query_fuel_phase() { return fuel_phase; }
int query_ash_phase() { return ash_phase; }
void set_fuel_phase(int n) { fuel_phase = n; }
void set_ash_phase(int n) { ash_phase = n; }
void create()
{
    set_name("青銅丹爐", ({"bronze furnace", "furnace"}));
    set("unit", "座"); set("value", 10000);
    set("long", "一座可容納丹鼎與松柴的青銅丹爐。\n");
    set_weight(5000); set_max_encumbrance(16000); setup();
    HEAT_D->track_furnace(this_object());
}
int accept_object(object me, object ob) { return 1; }
