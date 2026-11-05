/* CUSTOM RACE OBJECT: 雨師妾的小蛇（綁定在主人身上的裝備）
 *
 * 經驗與斑點記在主人身上（rainner/snake/<顏色>），本物件只是依記錄產生的
 * 裝備；每次登入由雨師妾種族重建。能力依斑點線性成長，斑點 120 時達到上限。
 * 不能丟棄（丟下會自己爬回來）、給人、放進容器或被偷（偷牠一定被發現），
 * 主人昏倒、死亡時也拿不走、不會掉進屍體，其他人無法穿戴。
 */
#include <ansi.h>
#include <armor.h>

#define MAX_SPOTS 120

private string owner_id, color;
private int destructing;

// 顏色 : ({ 中文名, 英文名, 部位, 滿級能力 })
// 青蛇的 spell / magic_ability 是咒術技巧 / 法術技巧的額外附加（query_ability）。
private mapping snake_data = ([
    "white":  ({ "白蛇", "white viper",  "waist_eq",
        ([ "con":5, "armor":25, "wittiness":25, "armor_vs_ice":100 ]) }),
    "black":  ({ "黑蛇", "black viper",  "leg_eq",
        ([ "dex":5, "intimidate":30, "attack":50, "awarness":100 ]) }),
    "green":  ({ "青蛇", "green viper",  "head_eq",
        ([ "spi":5, "spell":20, "magic_ability":20, "armor_vs_wind":100 ]) }),
    "red":    ({ "赤蛇", "red viper",    "hand_eq",
        ([ "str":5, "damage":20, "intimidate":30, "armor_vs_fire":100 ]) }),
    "yellow": ({ "黃蛇", "yellow viper", "neck_eq",
        ([ "cps":5, "move":50, "defense":50, "armor_vs_lightning":100 ]) }),
]);

void create() {
    seteuid(getuid());
    set_name("小蛇", ({ "viper", "snake" }));
    set_weight(0);
    setup();
    set("unit", "條");
    set("value", 0);
    set("no_drop", "你的小蛇緊緊纏在你身上，不肯離開。\n");
}

// drop 時：小蛇被放到地上，又自己爬回主人身上（使用者定案，2026-11-05）。
// 由 cmds/std/drop.c 與 daemon/misc/heat_commands.c 的 drop 呼叫。
int crawl_back(object me) {
    message_vision("$N將" + name() + "放到地上﹐" + name() + "扭了扭身子﹐又自己爬回$N身上。\n", me);
    return 1;
}

int query_spots() {
    object owner = environment();
    int exp, n;
    if (!owner || !color) return 0;
    exp = owner->query("rainner/snake/" + color);
    // 斑點 n 需要累計經驗 n^2 x 10。
    while (n < MAX_SPOTS && (n + 1) * (n + 1) * 10 <= exp) n++;
    return n;
}

// 依目前斑點重算能力；若正在穿戴，先脫下再穿上讓新數值生效。
void refresh() {
    mapping full, now = ([]);
    string key, part;
    int spots, worn;

    if (!color) return;
    part = snake_data[color][2];
    full = snake_data[color][3];
    spots = query_spots();
    foreach (key in keys(full)) now[key] = full[key] * spots / MAX_SPOTS;

    worn = query("equipped") != 0;
    if (worn) { destructing = 1; this_object()->unequip(); destructing = 0; }
    set("apply_armor/" + part, now);
    set("long", "一條雨師妾以自己鮮血餵養長大的" + snake_data[color][0]
        + "，身上已經有 " + spots + " 個斑點"
        + (spots >= MAX_SPOTS ? "，成長已達極限" : "")
        + "。\n可以用 feed 指令餵養牠。\n");
    if (worn) this_object()->wear();
}

void set_owner(object owner, string c) {
    owner_id = owner->query("id");
    color = c;
    set_name(snake_data[color][0], ({ snake_data[color][1], "viper", "snake" }));
    set("rainner_snake", color);
    set("wear_as", snake_data[color][2]);
}

string query_owner_id() { return owner_id; }

// 只能待在主人身上。
varargs int move(mixed dest, int silently) {
    if (stringp(dest)) dest = find_object(dest);
    if (!objectp(dest) || !owner_id || dest->query("id") != owner_id || !userp(dest))
        return notify_fail(name() + "緊緊纏在主人身上，不肯離開。\n");
    return ::move(dest, silently);
}

varargs int wear(string on_part) {
    object owner = environment();
    int ok;
    if (!owner || owner->query("id") != owner_id)
        return notify_fail(name() + "只認得自己的主人。\n");
    if (!query("apply_armor")) refresh();
    ok = ::wear(on_part);
    if (ok) owner->set("rainner/worn/" + color, 1);
    return ok;
}

int unequip() {
    object owner = environment();
    int ok = ::unequip();
    // 登出或物件銷毀時會自動脫下，這時不要記成「主人脫下了」。
    if (ok && !destructing && owner && owner->query("id") == owner_id)
        owner->set("rainner/worn/" + color, 0);
    return ok;
}

void remove(string euid) {
    destructing = 1;
    ::remove(euid);
}
