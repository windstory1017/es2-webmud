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

int is_heat_ash() { return 1; }
void create()
{
    set_name("焦灰", ({"ash"}));
    set("unit", "些"); set("base_unit", "份");
    set("base_weight", 1); set("base_value", 0);
    set("long", "一堆焦灰。\n");
    setup();
}
