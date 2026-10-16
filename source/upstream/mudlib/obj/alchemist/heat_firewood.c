#include "/daemon/misc/heat_b.h"
inherit ITEM;

private int amount = 1;
int query_amount() { return amount; }
void set_amount(int n)
{
    if (n < 0) error("Negative stack amount.\n");
    HEAT_D->invalidate_inventory();
    amount = n;
    set_weight(n * query("base_weight"));
    if (!n) destruct(this_object());
}
void add_amount(int n) { set_amount(amount + n); }
int render_value() { return amount * query("base_value"); }
void setup()
{
    ::setup();
    set("value", (: render_value :));
    set_amount(amount);
}
varargs string short(int raw)
{
    if (raw) return ::short(raw);
    return chinese_number(amount) + query("base_unit") + ::short(raw);
}
varargs int move(mixed dest, int silent)
{
    object old;
    old = environment();
    if (!::move(dest, silent)) return 0;
    HEAT_D->arrived(this_object(), old);
    return 1;
}


private int burning, phase;
int is_heat_wood() { return 1; }
int query_burning() { return burning; }
int query_burn_phase() { return phase; }
void set_burn_phase(int n) { phase = n; }
void set_burning(int state)
{
    HEAT_D->invalidate_inventory();
    burning = state;
    if (!state) phase = 0;
    if (state)
        set_name("燃燒中的松柴", ({"burning firewood", "firewood", "pine firewood"}));
    else set_name("松柴", ({"firewood", "pine firewood"}));
    HEAT_D->track_wood(this_object());
    HEAT_D->refresh_holder(environment());
    HEAT_D->refresh_furnace(environment());
}
void create()
{
    seteuid(getuid());
    set("unit", "捆"); set("base_unit", "根");
    set("base_weight", 120); set("base_value", 5);
    set("long", "一捆松木柴。\n");
    set_burning(0);
    setup();
}
void init()
{
    object me;
    me = this_player();
    if (!me || (environment() != me && environment() != environment(me))) return;
    add_action("do_ignite", "ignite");
    add_action("do_extinguish", "extinguish");
}
int do_ignite(string arg)
{
    if (arg != "firewood" && arg != "burning firewood") return 0;
    if (!HEAT_D->select_wood(this_player())) return 0;
    return HEAT_D->enqueue(this_player(), "ignite", arg);
}
int do_extinguish(string arg)
{
    if (arg != "firewood" && arg != "burning firewood") return 0;
    if (!HEAT_D->select_wood(this_player())) return 0;
    return HEAT_D->enqueue(this_player(), "extinguish", arg);
}
// Called by the canonical liquid move/dissolve protocol, no cross-layer scan.
void dissolve_with(object liquid)
{
    if (liquid && liquid->id("water") && liquid->query_volume() > 0
    && environment(liquid) == environment()) HEAT_D->wet(this_object());
}
