// Longtu HEAT B: one scheduler owns heat, ash, fuel and holder clocks.
#include "heat_b.h"
#include <ansi.h>

private object *furnaces = ({}), *woods = ({});
private mapping holders = ([]);
private mixed *queue = ({}), *scents = ({});
private mapping notes = ([]), seen_scents = ([]);
private mapping ashed_tokens = ([]);
private mapping paused = ([]);
private object actor;
private int frame, in_pulse;
private mapping inventory_cache=([]), mixture_cache=([]);


private int profiling, inventory_calls, inventory_items, predicate_calls;
void profile_start() { inventory_calls=inventory_items=predicate_calls=0; profiling=1; }
mapping profile_result() { return (["inventory_calls":inventory_calls,"inventory_items":inventory_items,"predicate_calls":predicate_calls]); }
object *measured_inventory(object env) {
    object *items;
    items=all_inventory(env);
    if(profiling) { inventory_calls++; inventory_items+=sizeof(items); }
    return items;
}

void refresh_holder(object who);
void refresh_furnace(object furnace);
void merge_product(object ob);
void wet(object wood);
void flush_furnace(object furnace);
void flush_holder(object who);
void flush_wood(object wood);
void pulse();

void create() { seteuid(getuid()); }
void heart_beat() { pulse(); }
object *inventory(object env) {
    object *items;
    if(!env) return ({});
    if(!in_pulse) return measured_inventory(env);
    items=inventory_cache[env];
    if(!arrayp(items)) { items=measured_inventory(env); inventory_cache[env]=items; }
    return items - ({0});
}
void invalidate_inventory() { inventory_cache=([]); mixture_cache=([]); }
int query_frame() { return frame; }
int query_holder_phase(object who) { return undefinedp(holders[who]) ? 0 : holders[who]; }
void track_furnace(object ob)
{
    if (clonep(ob) && ob->query_burning() && member_array(ob, furnaces) < 0) { furnaces += ({ob}); set_heart_beat(1); }
}
void track_wood(object ob)
{
    if (clonep(ob) && ob->query_burning() && environment(ob) && !environment(ob)->is_heat_furnace() && member_array(ob, woods) < 0) { woods += ({ob}); set_heart_beat(1); }
}
object room_of(object ob)
{
    if (!ob) return 0;
    while (environment(ob)) ob = environment(ob);
    return ob;
}
object furnace_of(object ob)
{
    object env;
    if (!ob) return 0;
    if (ob->is_heat_furnace()) return ob;
    env = environment(ob);
    if (env && env->is_heat_furnace()) return env;
    if (env && env->is_heat_reactor() && environment(env)
    && environment(env)->is_heat_furnace()) return environment(env);
    return 0;
}
object *mixtures(object furnace)
{
    object pot, ob;
    object *result = ({});
    if (!furnace) return result;
    if(arrayp(mixture_cache[furnace])) return mixture_cache[furnace] - ({0});
    foreach (pot in inventory(furnace))
        if (pot->is_heat_reactor())
            foreach (ob in inventory(pot))
                if (ob->is_heat_mixture()) result += ({ob});
    mixture_cache[furnace]=result;
    return result;
}
int burning_count(object env)
{
    if(profiling) predicate_calls++;
    object ob;
    int n;
    if (!env) return 0;
    foreach (ob in inventory(env))
        if (ob->is_heat_wood() && ob->query_burning()) n += ob->query_amount();
    return n;
}
void refresh_holder(object who)
{
    if (!who || !userp(who)) return;
    if (!burning_count(who) || who->query("life_form") == "ghost")
        map_delete(holders, who);
    else if (undefinedp(holders[who])) holders[who] = 0;
}
void refresh_furnace(object furnace)
{
    if(profiling) predicate_calls++;
    object ob;
    int n;
    if (!furnace || !furnace->is_heat_furnace()) return;
    n = burning_count(furnace);
    if (!n && furnace->query_burning()) {
        furnace->set_fuel_phase(0);
        furnace->set_ash_phase(0);
        foreach (ob in mixtures(furnace)) { ob->set_heat_phase(0); ob->suspend_phase(); }
    }
    if(n && !furnace->query_burning()) {
        furnace->set_burning(1);
        foreach(ob in mixtures(furnace)) ob->resume_phase();
    } else furnace->set_burning(n > 0);
    track_furnace(furnace);
}
object select_wood(object me)
{
    object ob;
    if (!me) return 0;
    foreach (ob in inventory(me)) if (ob->is_heat_wood()) return ob;
    if (environment(me))
        foreach (ob in inventory(environment(me)))
            if (ob->is_heat_wood()) return ob;
    return 0;
}
void begin_operation(object me) { invalidate_inventory(); actor = me; notes = ([]); }
void note(string kind) { if (actor) notes[kind] = 1; }
void cancel_split_note() { map_delete(notes, "split"); }
void end_operation()
{
    if (actor) {
        if (notes["split"]) tell_object(actor, replace_string(SPLIT, "你", gender_self(actor)));
        if (notes["valid-mix"]) tell_object(actor, MIXED);
        if (notes["unknown-mix"]) tell_object(actor, MIXED);
        if (notes["merge"] && !notes["valid-mix"])
            tell_object(actor, gender_self(actor) + "將相同的配方合而為一。\n");
    }
    notes = ([]); actor = 0;
}
void wet(object wood)
{
    object env;
    if (!wood || !wood->query_burning()) return;
    env = environment(wood);
    flush_holder(env);
    flush_wood(wood);
    if (!wood) return;
    wood->set_burning(0);
    refresh_furnace(env);
    refresh_holder(env);
    if (actor && !notes["water"]) {
        tell_object(actor, WATER_OUT);
        notes["water"] = 1;
    } else if (!actor && this_player()) tell_object(this_player(), WATER_OUT);
}
string product_key(object ob) { return ob ? ob->query_heat_product() : 0; }
void merge_product(object ob)
{
    object other;
    if (!environment(ob)) return;
    foreach (other in inventory(environment(ob)))
        if (other != ob && (base_name(ob) == base_name(other)
        || (product_key(ob) && product_key(ob) == product_key(other)))) {
            ob->add_amount(other->query_amount());
            destruct(other);
        }
}
void arrived(object ob, object old)
{
    object env, other;
    int amount, sum, phase, burning;
    string key;
    invalidate_inventory();
    env = environment(ob);
    if (!env) return;
    if (ob->is_heat_mixture()) {
        foreach (other in inventory(env)) {
            if (other == ob || !other->is_heat_mixture()) continue;
            amount = ob->query_amount() + other->query_amount();
            key = ob->query_recipe();
            if (key != "unknown" && key == other->query_recipe()
            && ob->query_heat() > 0 && other->query_heat() > 0) {
                sum = ob->query_amount() * ob->query_heat()
                    + other->query_amount() * other->query_heat();
                ob->set_heat(sum / amount);
                note("merge");
            } else {
                note(key == "unknown" && other->query_recipe() == "unknown"
                    ? "unknown-mix" : "valid-mix");
                ob->configure("unknown");
            }
            ob->set_heat_phase(0);
            destruct(other);
            ob->set_amount(amount);
        }
    } else if (ob->is_heat_wood()) {
        burning = ob->query_burning(); phase = ob->query_burn_phase();
        if (burning && old && old->is_heat_furnace()) phase = old->query_fuel_phase();
        // Rooms and characters are not "new containers" for rule 109.
        if (burning && env != old && !env->is_character()
        && !inherits(ROOM, env) && !env->is_room()) phase = 0;
        foreach (other in inventory(env)) {
            if (other == ob || !other->is_heat_wood()) continue;
            if (other->query_burning()) {
                phase = other->query_burn_phase(); burning = 1;
            }
            amount = ob->query_amount() + other->query_amount();
            destruct(other); ob->set_amount(amount);
        }
        ob->set_burning(burning); ob->set_burn_phase(phase);
        foreach (other in inventory(env))
            if (other != ob && other->id("water") && other->query_volume() > 0)
                wet(ob);
        track_wood(ob);
    } else if (ob->is_heat_ash() || ob->query_heat_product()) merge_product(ob);
    refresh_furnace(old); refresh_furnace(env);
    refresh_holder(old); refresh_holder(env);
}
object split_stack(object ob, int count)
{
    object part;
    int zero;
    if (count <= 0 || count >= ob->query_amount()) return 0;
    part = new(base_name(ob));
    if (ob->is_heat_mixture()) {
        zero = ob->query_recipe() != "unknown" && ob->query_heat() == 0;
        part->configure(zero ? "unknown" : ob->query_recipe());
        part->set_heat(ob->query_heat());
        part->set_heat_phase(ob->query_heat_phase());
        if (zero) { ob->configure("unknown"); note("split"); }
    }
    // A split piece of firewood starts cold, as its create() specifies.
    part->set_amount(count);
    ob->add_amount(-count);
    return part;
}
void scent(object furnace, string text, object mature)
{
    // Keep identity after successful preparation/drop. Only actual ash cancels aroma.
    scents += ({({room_of(furnace), text, mature ? file_name(mature) : 0})});
}
void flush_scents()
{
    mixed *event;
    string key;
    object who;
    foreach (event in scents) {
        if (!event[0] || (event[2] && ashed_tokens[event[2]])) continue;
        key = file_name(event[0]) + event[1];
        if (seen_scents[key]) continue;
        seen_scents[key] = 1;
        foreach (who in inventory(event[0]))
            if (userp(who)) tell_object(who, event[1]);
    }
    scents = ({});
}
void make_ash(object ob, object furnace)
{
    object ash, dest;
    int limit, slots;
    invalidate_inventory();
    dest = environment(ob);
    ashed_tokens[file_name(ob)] = 1;
    ash = new(HEAT_ITEM "heat_ash"); ash->set_amount(ob->query_amount());
    scent(furnace, ob->query_recipe() == "unknown" ? UNKNOWN_ASH : CHARRED, 0);
    // Passive conversion must not be rejected by active placement capacity.
    limit = dest->query_max_encumbrance(); slots = dest->query_max_inventory();
    dest->set_max_encumbrance(dest->query_encumbrance() + ash->weight() + 1);
    dest->set_max_inventory(-1);
    if (!ash->move(dest)) {
        dest->set_max_encumbrance(limit); dest->set_max_inventory(slots);
        destruct(ash); error("Passive ash delivery failed.\n");
    }
    dest->set_max_encumbrance(limit); dest->set_max_inventory(slots);
    destruct(ob);
}
void flush_furnace(object furnace)
{
    if(profiling) predicate_calls++;
    object ob;
    mixed *row;
    int n, old;
    if (!furnace || !furnace->query_burning()) return;
    n = burning_count(furnace);
    foreach (ob in mixtures(furnace)) {
        if (ob->query_recipe() == "unknown" || ob->query_heat_phase() < 10) continue;
        row = (HB "heat_recipes")->query_recipes()[ob->query_recipe()];
        old = ob->query_heat();
        ob->set_heat(old + (HB "heat_recipes")->increment(ob->query_recipe(), n));
        ob->set_heat_phase(0);
        if (old < row[4] && ob->query_heat() >= row[4]) scent(furnace, AROMA, ob);
    }
    if (furnace->query_ash_phase() >= 3) {
        furnace->set_ash_phase(0);
        foreach (ob in mixtures(furnace)) {
            row = (HB "heat_recipes")->query_recipes()[ob->query_recipe()];
            if (!row || ob->query_heat() > row[5] || n * 10 > row[3])
                make_ash(ob, furnace);
        }
    }
    if (furnace->query_fuel_phase() >= 10) {
        furnace->set_fuel_phase(0);
        foreach (ob in inventory(furnace))
            if (ob->is_heat_wood() && ob->query_burning()) {
                ob->add_amount(-1); break;
            }
        refresh_furnace(furnace);
    }
}
void flush_holder(object who)
{
    object viewer;
    int n;
    string self;
    if (!who || undefinedp(holders[who]) || holders[who] < 5) return;
    if (who->query("life_form") == "ghost") { map_delete(holders, who); return; }
    n = burning_count(who);
    if (!n) { map_delete(holders, who); return; }
    holders[who] = 0;
    // Current first, then damaged ceiling: canonical damage_stat clamps current.
    who->consume_stat("kee", n * 2, who);
    who->damage_stat("kee", n, who);
    self = gender_self(who);
    tell_object(who, HIR + "燃燒中的松柴持續灼燙著" + self + "，" + self
        + "咬牙忍住那股熱辣辣的灼傷，額際漸漸滲出細汗。\n" + NOR);
    if (environment(who)) foreach (viewer in inventory(environment(who)))
        if (viewer != who && userp(viewer) && who->visible(viewer))
            tell_object(viewer, RED + gender_self(viewer) + "在一陣火光中發現，"
                + who->name() + "正緊咬牙關、眉頭微蹙，額際佈滿細汗。\n" + NOR);
}
void flush_wood(object wood)
{
    if(profiling) predicate_calls++;
    object env;
    if (!wood || !wood->query_burning()) return;
    env = environment(wood);
    if (env && env->is_heat_furnace()) { flush_furnace(env); return; }
    flush_holder(env);
    if (wood->query_burn_phase() >= 10) {
        wood->set_burn_phase(0); wood->add_amount(-1);
        refresh_holder(env);
    }
}
// Called before a command mutates a furnace; cycles resolve by timed events first.
void before_take(object ob, int count)
{
    object furnace;
    furnace = furnace_of(ob);
    if (ob->is_heat_wood()) {
        if (furnace && (count == ob->query_amount() || furnace->query_fuel_phase() >= 10))
            flush_furnace(furnace);
        else if (!furnace) flush_wood(ob);
    } else if (furnace && (ob->is_heat_mixture() || ob->is_heat_reactor()))
        flush_furnace(furnace);
}
int has_pending(object me)
{
    mixed *entry;
    foreach (entry in queue) if (entry[0] == me) return 1;
    return 0;
}
int handles(object me, string verb, string arg)
{
    object ob, env, dest;
    string item, target;
    int count;
    if (!arg || !me || !environment(me)) return 0;
    if (has_pending(me)) return 1;
    env = me; item = arg;
    if (verb == "get") {
        env = environment(me);
        if (sscanf(arg, "%s from %s", item, target) == 2) {
            env = present(target, me);
            if (!env) env = present(target, environment(me));
        }
    } else if (verb == "put") {
        if (sscanf(arg, "%s in %s", item, target) != 2) return 0;
        dest = present(target, me);
        if (!dest) dest = present(target, environment(me));
        if (dest && (dest->is_heat_furnace() || dest->is_heat_reactor()
        || burning_count(dest))) return 1;
    }
    if (!env) return 0;
    if (env->is_heat_furnace() || env->is_heat_reactor()) return 1;
    if (item == "all") {
        foreach (ob in inventory(env))
            if (ob->is_heat_wood() || ob->is_heat_mixture() || ob->is_heat_ash()
            || ob->is_heat_furnace() || ob->is_heat_reactor() || product_key(ob)) return 1;
        return 0;
    }
    if (sscanf(item, "%d %s", count, target) == 2) item = target;
    ob = present(item, env);
    return ob && (ob->is_heat_wood() || ob->is_heat_mixture() || ob->is_heat_ash()
        || ob->is_heat_furnace() || ob->is_heat_reactor() || product_key(ob));
}
int has_water(object env)
{
    object ob;
    if (!env) return 0;
    foreach (ob in inventory(env)) if (ob->id("water") && ob->query_volume() > 0) return 1;
    return 0;
}
void on_death(object who) { map_delete(holders, who); map_delete(paused, who); }
void on_disconnect(object who)
{
    // Close the player's current open frame before the canonical net_dead hook.
    if (!undefinedp(holders[who]) && !paused[who]) holders[who]++;
    flush_holder(who); paused[who] = 1;
}
void on_reconnect(object who) { map_delete(paused, who); refresh_holder(who); }
int enqueue(object me, string verb, string arg)
{
    if (!me || me != this_player()) return 0;
    queue += ({({me, verb, arg})});
    set_heart_beat(1);
    return 1;
}
void pulse()
{
    object f, ob, who;
    mixed *action, *batch;
    mixed err;
    in_pulse=1;
    invalidate_inventory();
    frame++; seen_scents = ([]); scents = ({}); ashed_tokens = ([]);
    furnaces -= ({0}); woods -= ({0});
    foreach (f in furnaces) {
        refresh_furnace(f);
        if (!f->query_burning()) continue;
        f->set_fuel_phase(f->query_fuel_phase() + 1);
        f->set_ash_phase(f->query_ash_phase() + 1);

    }
    foreach (ob in woods) if (ob->query_burning() && environment(ob)) {
        refresh_holder(environment(ob));
        if (!environment(ob)->is_heat_furnace()) ob->set_burn_phase(ob->query_burn_phase() + 1);
    }
    foreach (who in keys(holders)) {
        if (!who) { map_delete(holders, who); continue; }
        refresh_holder(who);
        if (!undefinedp(holders[who]) && interactive(who) && !paused[who]) holders[who]++;
    }
    batch = queue; queue = ({});
    foreach (action in batch) {
        if (!action[0]) continue;
        begin_operation(action[0]);
        err = catch((HB "heat_commands")->execute(action[0], action[1], action[2]));
        end_operation();
        if (err) {
            tell_object(action[0], "這次操作未能完成。\n");
            log_file("heat_b", sprintf("%O\n", err));
        }
    }
    foreach (who in keys(holders)) if (who) flush_holder(who);
    foreach (f in furnaces) if (f) flush_furnace(f);
    foreach (ob in woods) if (ob && environment(ob) && !environment(ob)->is_heat_furnace()) flush_wood(ob);
    flush_scents();
    furnaces=filter_array(furnaces, (: $1 && $1->query_burning() :));
    woods=filter_array(woods, (: $1 && $1->query_burning() && environment($1) :));
    foreach(who in keys(paused)) if(!who || undefinedp(holders[who])) map_delete(paused,who);
    invalidate_inventory();
    in_pulse=0;
    if(!sizeof(furnaces) && !sizeof(woods) && !sizeof(holders) && !sizeof(queue)) set_heart_beat(0);
}
