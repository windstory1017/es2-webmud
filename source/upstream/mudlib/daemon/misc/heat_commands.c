#include "heat_b.h"

void create() { seteuid(getuid()); }

// 自己看到的訊息依性別顯示你／妳（2026-10-15 使用者決定）。
int fail(object me, string text) { tell_object(me, replace_string(text, "你", gender_self(me)) + "\n"); return 0; }
object direct_container(object me, string id)
{
    object ob;
    ob = present(id, me);
    if (!ob && environment(me)) ob = present(id, environment(me));
    if (ob && (ob->is_character() || living(ob))) return 0;
    return ob;
}
int put_limit(object me, object ob, object dest, int count)
{
    int weight;
    if (dest->is_heat_reactor() && ob->is_heat_mixture()
    && dest->mixture_count() + count > 5)
        return fail(me, "丹鼎最多只能容納五份配方。");
    weight = ob->query_amount() ? ob->query_weight() * count / ob->query_amount() : ob->weight();
    // Canonical move() exempts moves to an ancestor from capacity checks.
    object ancestor;
    ancestor = environment(ob);
    while (ancestor) {
        if (ancestor == dest) return 1;
        ancestor = environment(ancestor);
    }
    if (weight > dest->query_max_encumbrance() - dest->query_encumbrance())
        return fail(me, dest->name() + "似乎裝不下了。");
    return 1;
}
int transfer(object me, string verb, object ob, object dest, int count)
{
    object old, part, guard;
    object *guards;
    string label, unit;
    int total, partial, old_heat, old_phase;
    string old_recipe;
    mixed denied;
    if (!ob || !dest) return 0;
    old = environment(ob);
    if (verb == "get") {
        if (ob->is_character()) return fail(me, "不行!");
        if (old && old->hold_object(ob)) return fail(me, "這樣東西拿不出來。");
        if (denied = ob->query("no_get"))
            return fail(me, stringp(denied) ? denied : "這個東西拿不起來。");
        guards = ob->query_temp("guarded");
        if (arrayp(guards)) foreach (guard in guards)
            if (guard && guard != me && living(guard) && environment(guard) == environment(me))
                return fail(me, guard->name() + "正守在" + ob->name() + "一旁﹐防止任何人拿走。");
        if (me->over_encumbranced()) return fail(me, "你已經負荷過重了﹗");
    } else {
        if (denied = ob->query("no_drop"))
            return fail(me, stringp(denied) ? denied : "這樣東西不能隨意丟棄。");
        if (verb == "put" && (ob == dest || ob->query("no_get")))
            return fail(me, "這樣東西無法被移動。");
        // 神農之術 S-12：封住的丹鼎放不進東西。
        if (verb == "put" && dest->is_heat_reactor() && stringp(dest->query_seal()))
            return fail(me, "你的" + ob->name() + "放不進" + dest->name() + "。");
    }
    total = ob->query_amount();
    if (!total) total = 1;
    if (!count) count = total;
    if (count < 1 || count > total) return fail(me, "沒有那麼多的" + ob->name() + "。");
    partial = count < total;
    // The due timers may consume a last piece or ash a target. Revalidate below.
    if (verb == "get" || verb == "drop") HEAT_D->before_take(ob, count);
    if (ob && ob->is_heat_wood() && verb == "put" && HEAT_D->has_water(dest)) {
        HEAT_D->flush_holder(me); HEAT_D->flush_wood(ob);
    }
    if (!ob) return fail(me, "這裡沒有這樣東西。");
    if (ob->query_amount() && count > ob->query_amount()) {
        if (partial) return fail(me, "沒有那麼多的" + ob->name() + "。");
        count = ob->query_amount();
    }
    partial = ob->query_amount() && count < ob->query_amount();
    if (verb != "drop" && !put_limit(me, ob, dest, count)) return 0;
    if (verb == "put" && !dest->is_heat_reactor() && !dest->accept_object(me, ob))
        return fail(me, "這樣東西放不進去。");
    if (partial && ob->query("equipped")) return fail(me, "你沒有辦法取下這樣東西。");
    label = ob->name(); unit = ob->query("unit");
    part = ob;
    if (partial) {
        if (ob->is_heat_mixture()) {
            old_recipe = ob->query_recipe(); old_heat = ob->query_heat();
            old_phase = ob->query_heat_phase();
        }
        if (ob->is_heat_mixture() || ob->is_heat_wood() || ob->is_heat_ash()
        || ob->query_heat_product()) part = HEAT_D->split_stack(ob, count);
        else {
            part = new(base_name(ob)); part->set_amount(count); ob->add_amount(-count);
        }
    }
    if (verb == "drop" && (part->is_heat_mixture() || part->is_heat_ash())) {
        message_vision("$N丟下一" + unit + label + "。\n", me);
        destruct(part);
    } else {
        if (!part->move(dest)) {
            if (partial) {
                ob->add_amount(part->query_amount()); destruct(part);
                if (old_recipe) {
                    ob->configure(old_recipe); ob->set_heat(old_heat); ob->set_heat_phase(old_phase);
                    HEAT_D->cancel_split_note();
                }
            }
            return fail(me, "這次取放未能完成。");
        }
        if (part && HEAT_D->product_key(part)) HEAT_D->merge_product(part);
        if (verb == "put")
            message_vision("$N將一" + unit + label + "放進" + dest->name() + "。\n", me);
        else if (verb == "get") {
            message_vision("$N" + (old == environment(me) ? "撿起" : "從" + old->name() + "中拿出")
                + "一" + unit + label + "。\n", me);
            if (me->is_fighting()) me->start_busy(partial ? 3 : 1);
        } else {
            message_vision("$N丟下一" + unit + label + "。\n", me);
            // Finished pills retain the canonical wizard/value drop behaviour.
            if (part && !part->is_heat_wood() && (wizardp(me)
            || (!part->query("value") && !part->value()))) destruct(part);
            if (part) part->delete("keep_this");
        }
    }
    HEAT_D->refresh_furnace(old);
    HEAT_D->refresh_holder(old);
    HEAT_D->refresh_holder(me);
    return 1;
}
int fire(object me, int ignite)
{
    object wood, where, viewer;
    int held;
    string text;
    wood = HEAT_D->select_wood(me);
    if (!wood) return 0;
    where = environment(wood); held = where == me;
    if (!ignite) {
        HEAT_D->flush_holder(me); HEAT_D->flush_wood(wood);
        if (!wood) return fail(me, "這裡沒有這樣東西。");
    }
    if (wood->query_burning() == ignite) {
        text = ignite ? "已點燃。" : "尚未點燃。";
        tell_object(me, (held ? gender_self(me) + "身上的松柴" : "地上的松柴") + text + "\n");
        return 1;
    }
    wood->set_burning(ignite);
    wood->set_burn_phase(0);
    text = ignite ? "點燃。" : "熄滅。";
    tell_object(me, gender_self(me) + "將" + (held ? "身上" : "地上") + "的松柴" + text + "\n");
    if (!held && environment(me)) foreach (viewer in all_inventory(environment(me)))
        if (viewer != me && userp(viewer) && me->visible(viewer))
            tell_object(viewer, gender_self(viewer) + "看見" + me->name() + "將地上的松柴" + text + "\n");
    return 1;
}
int prepare(object me)
{
    object ob, product;
    object *source = ({}), *products = ({});
    string key;
    string *order = ({});
    mapping totals = ([]), rows, units = ([]);
    int low, unknown, limit, slots, weight;
    mixed err;
    rows = (HB "heat_recipes")->query_recipes();
    foreach (ob in all_inventory(me)) if (ob->is_heat_mixture()) {
        source += ({ob}); key = ob->query_recipe();
        if (key == "unknown") unknown = 1;
        else {
            if (ob->query_heat() < rows[key][4]) low = 1;
            if (undefinedp(totals[key])) { totals[key] = 0; order += ({key}); }
            totals[key] += ob->query_amount();
        }
    }
    // Canonical prepare/no-mixture handler: canonicalrepo未找到. Return unhandled.
    if (!sizeof(source)) return 0;
    if (unknown) return fail(me, "不知名配方無法煉成丹藥。");
    if (low) return fail(me, "配方火候不足，無法煉成丹藥。");
    // Instantiate every product before consuming any mixture.
    foreach (key in order) {
        err = catch(product = new((HB "heat_recipes")->product_path(key)));
        if (err || !product) {
            foreach (ob in products) if (ob) destruct(ob);
            return fail(me, "這次煉製未能完成。");
        }
        product->set_amount(totals[key]); products += ({product}); weight += product->weight();
        // 量詞讀成品檔 base_unit（2026-10-15 使用者決定統一為「粒」）。
        units[key] = stringp(product->query("base_unit")) ? product->query("base_unit") : "粒";
    }
    limit = me->query_max_encumbrance(); slots = me->query_max_inventory();
    me->set_max_encumbrance(me->query_encumbrance() + weight + 1);
    me->set_max_inventory(-1);
    foreach (ob in products) {
        if (!ob->move(me)) {
            me->set_max_encumbrance(limit); me->set_max_inventory(slots);
            error("Unexpected canonical product move failure.\n");
        }
    }
    me->set_max_encumbrance(limit); me->set_max_inventory(slots);
    foreach (ob in source) destruct(ob);
    foreach (key in order)
        tell_object(me, gender_self(me) + "將" + chinese_number(totals[key]) + "份" + rows[key][0] + "配方製作成"
            + chinese_number(totals[key]) + units[key] + rows[key][0] + "。\n");
    return 1;
}
int execute(object me, string verb, string arg)
{
    object env, dest, ob;
    object *inv;
    string item, target;
    int count;
    if (!me || !environment(me)) return 0;
    if (verb == "ignite" || verb == "extinguish") return fire(me, verb == "ignite");
    if (verb == "prepare") return prepare(me);
    if (me->is_busy()) return fail(me, "你上一個動作還沒有完成﹗");
    if (!arg) return fail(me, "你要移動什麼東西﹖");
    if (verb == "get") {
        if (me->query("life_form") == "ghost") return fail(me, "你現在是鬼魂狀態，無法拾取物品。");
        if (!me->query_stat_maximum("HP")) return fail(me, "你沒有形體﹐無法拾取物品。");
        if (sscanf(arg, "%s from %s", item, target) == 2) {
            env = direct_container(me, target);
            if (!env) return fail(me, "這裡沒有這樣東西。");
        } else { item = arg; env = environment(me); }
        dest = me;
    } else if (verb == "put") {
        if (sscanf(arg, "%s in %s", item, target) != 2)
            return fail(me, "你要將什麼東西放進哪裡﹖");
        env = me; dest = direct_container(me, target);
        if (!dest) return fail(me, "這裡沒有這樣東西。");
    } else { item = arg; env = me; dest = environment(me); }
    // 神農之術 S-12：封住的丹鼎拿不出東西；不說明原因，只顯示 Ok.
    if (verb == "get" && env->is_heat_reactor() && stringp(env->query_seal())) {
        tell_object(me, "Ok.\n");
        return 1;
    }
    if (item == "all") {
        if (verb == "get" && me->is_fighting()) return fail(me, "你還在戰鬥中﹗只能一次拿一樣。");
        inv = all_inventory(env);
        foreach (ob in inv) if (ob && ob != dest && !(verb == "drop" && ob->query("keep_this")))
            transfer(me, verb, ob, dest, 0);
        return 1;
    }
    if (sscanf(item, "%d %s", count, target) == 2) {
        item = target;
        if (count < 1) return fail(me, "東西的數量至少是一個。");
    }
    ob = present(item, env);
    if (!ob) return fail(me, "這裡沒有這樣東西。");
    if (count && !ob->query_amount()) return fail(me, ob->name() + "不能被分開。");
    return transfer(me, verb, ob, dest, count);
}
