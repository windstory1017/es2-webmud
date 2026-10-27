#include "/daemon/misc/heat_b.h"
inherit ITEM;
// 丹爐方位（架爐）：docs/martial_arts/龍圖丹派_煉丹流程.md 第 3.4 節（A-01～A-14，2026-10-24）。
// 指令用八方，顯示用後天八卦位；方位只是位置，不影響火候與成丹。
#define DIRECTIONS ([ \
    "north": "坎", "south": "離", "east": "震", "west": "兌", \
    "northwest": "乾", "southwest": "坤", "northeast": "艮", "southeast": "巽", \
])
#define SET_NO_GET "這座丹爐已穩穩架在地上，得先請下來才能移動。\n"
private int burning, fuel_phase, ash_phase;
private string direction; // 架設的方位（north 等）；0＝沒有架設
int is_heat_furnace() { return 1; }
int query_burning() { return burning; }
void set_burning(int n) { burning = n; }
int query_fuel_phase() { return fuel_phase; }
int query_ash_phase() { return ash_phase; }
void set_fuel_phase(int n) { fuel_phase = n; }
void set_ash_phase(int n) { ash_phase = n; }
string query_direction() { return direction; }
string query_direction_name() { return direction ? DIRECTIONS[direction] + "位" : 0; }
void create()
{
    set_name("青銅丹爐", ({"bronze furnace", "furnace"}));
    set("unit", "座"); set("value", 10000);
    // 描述照使用者提供的原版文字（2026-10-24，保留指令說明，使用者確認）。
    set("long", @LONG
這是一個青銅所鑄成的丹爐，是一般方士煉製丹藥時用來燒火粹煉
的爐子，要用時必須先架設在(setup)地上，講究一些的會築一個丹
壇，不過只有煉仙丹大藥才需如此，平常只要挖個坑埋住，可以燒
就行。
煉丹完畢，記得將丹爐起出(dispose)帶走。
指令格式：setup <丹爐> toward <方位>
LONG
    );
    set_weight(5000); set_max_encumbrance(16000); setup();
    HEAT_D->track_furnace(this_object());
}
int accept_object(object me, object ob) { return 1; }
// A-12：房間裡顯示「青銅丹爐 (Bronze furnace) <坎位>」。
varargs string short(int raw)
{
    string str;
    str = ::short(raw);
    if (!raw && direction) str += " <" + query_direction_name() + ">";
    return str;
}
void init()
{
    object me;
    me = this_player();
    if (!me || (environment() != me && environment() != environment(me))) return;
    add_action("do_setup", "setup");
    add_action("do_dispose", "dispose");
}
// A-02：同一房間的某個方位是否已有架好的丹爐。
private int occupied(object room, string dir)
{
    object ob;
    foreach (ob in all_inventory(room))
        if (ob->is_heat_furnace() && ob->query_direction() == dir) return 1;
    return 0;
}
void set_up(string dir)
{
    direction = dir;
    set("no_get", SET_NO_GET);
}
void take_down()
{
    direction = 0;
    delete("no_get");
}
// setup <丹爐> toward <方位>
int do_setup(string arg)
{
    object me, room, ob;
    string item, dir, self, where;
    me = this_player();
    room = environment(me);
    if (!arg || !room) return 0;
    if (sscanf(arg, "%s toward %s", item, dir) != 2) { item = arg; dir = 0; }
    ob = present(item, me);
    if (!ob) ob = present(item, room);
    if (!ob || !ob->is_heat_furnace()) return 0;
    self = gender_self(me);
    if (ob->query_direction()) return notify_fail("這座丹爐早已架好了。\n");
    if (!dir) return notify_fail(self + "得先指明要將丹爐架在哪一個方位。\n");
    if (undefinedp(DIRECTIONS[dir])) return notify_fail("八方之中並無此一方位。\n");
    where = DIRECTIONS[dir] + "位";
    if (occupied(room, dir)) return notify_fail(where + "已架著一座丹爐了。\n");
    // A-04：丹爐在身上時先放到地上再架好。
    if (environment(ob) == me) {
        if (ob->query("no_drop")) return notify_fail("這樣東西不能隨意丟棄。\n");
        HEAT_D->before_take(ob, 0);
        if (!ob->move(room)) return notify_fail("這次取放未能完成。\n");
        HEAT_D->refresh_furnace(me);
        HEAT_D->refresh_holder(me);
        tell_object(me, self + "放下" + ob->name() + "，穩穩架在" + where
            + "，爐足落地生根，再難輕易挪動。\n");
    } else
        tell_object(me, self + "將" + ob->name() + "穩穩架在" + where
            + "，爐足落地生根，再難輕易挪動。\n");
    ob->set_up(dir);
    tell_room(room, me->name() + "將" + ob->name() + "穩穩架在" + where + "。\n", ({me}));
    return 1;
}
// dispose <丹爐>（例如 dispose furnace 2）
int do_dispose(string arg)
{
    object me, room, ob;
    string where;
    me = this_player();
    room = environment(me);
    if (!arg || !room) return 0;
    ob = present(arg, room);
    if (!ob || !ob->is_heat_furnace()) return 0;
    if (!ob->query_direction()) return notify_fail("這座丹爐並未架設。\n");
    where = ob->query_direction_name();
    ob->take_down();
    tell_object(me, gender_self(me) + "將" + where + "的" + ob->name() + "請了下來。\n");
    tell_room(room, me->name() + "將" + where + "的" + ob->name() + "請了下來。\n", ({me}));
    return 1;
}
