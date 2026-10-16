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
varargs string stack_short(int raw)
{
    if (raw) return ::short(raw);
    return chinese_number(amount) + query("base_unit") + ::short(raw);
}
void suspend_phase();
void resume_phase();
varargs int move(mixed dest, int silent)
{
    object old;
    old = environment();
    suspend_phase();
    if (!::move(dest, silent)) { resume_phase(); return 0; }
    HEAT_D->arrived(this_object(), old);
    resume_phase();
    return 1;
}

void configure(string key);
mixed *local_recipe() { return ({"金液丹","aquapill",90,180,420,540}); }
int local_value() { return 50; }
void create() { configure("aqua"); set("produce_param", (["min_heat_power":90,"max_heat_power":180,"min_heat":420,"max_heat":540])); }



private string recipe;
private int heat, phase, phase_frame, phase_active;
int is_heat_mixture() { return 1; }
string query_recipe() { return recipe; }
int query_heat() { return heat; }

int heated_here() {
    object pot, furnace;
    pot=environment();
    if (!pot || !pot->is_heat_reactor()) return 0;
    furnace=environment(pot);
    return furnace && furnace->is_heat_furnace() && furnace->query_burning();
}
int query_heat_phase() {
    return phase + (phase_active && heated_here() ? HEAT_D->query_frame()-phase_frame : 0);
}
void suspend_phase() { phase=query_heat_phase(); phase_frame=HEAT_D->query_frame(); phase_active=0; }
void resume_phase() { phase_frame=HEAT_D->query_frame(); phase_active=recipe!="unknown" && heated_here(); }
void set_heat_phase(int value) { phase=value; phase_frame=HEAT_D->query_frame(); }

void set_heat(int value) { heat = value; }
int query_grade()
{
    int g;
    if (recipe == "unknown") return 0;
    g = 10 * heat / local_recipe()[4];
    return g > 10 ? 10 : g;
}
void configure(string key)
{
    mixed *row;
    seteuid(getuid());
    recipe = key;
    row = local_recipe();
    set("unit", "些");
    set("base_unit", "份");
    set("base_weight", MIXTURE_WEIGHT);
    set("base_value", key == "unknown" ? 0 : local_value());
    if (key == "unknown") {
        set_name("不知名配方", ({"mixture of unknown composition", "mixture"}));
        set("long", "一些無法辨認種類的混合藥材。\n");
        heat = phase = phase_active = 0; phase_frame=HEAT_D->query_frame();
    } else {
        set_name(row[0] + "配方", ({"mixture of " + row[1], "mixture"}));
        set("long", "一份用於煉製" + row[0] + "的混合藥材。\n");
    }
    setup();
}
varargs string short(int raw)
{
    string text;
    text = stack_short(raw);
    if (!raw && heat > 0 && recipe != "unknown")
        text += "＜" + chinese_number(query_grade()) + "分火候＞";
    return text;
}
void init()
{
    if (this_player() && environment() == this_player())
        add_action("do_prepare", "prepare");
}
int do_prepare(string arg)
{
    if (arg != "mixture" || environment() != this_player()) return 0;
    return HEAT_D->enqueue(this_player(), "prepare", arg);
}
