/* CUSTOM HOME SYSTEM
 * 200-slot private persistent storage. One stored object stack consumes one slot.
 * WebMUD extension. Not part of taedlar/es2_mudlib canonical content.
 */
#include <ansi.h>

inherit ITEM;

object home_owner;

void set_home_owner(object ob) { home_owner = ob; }

void create() {
    set_name(HIY "私人儲物箱" NOR, ({ "home chest", "chest", "storage chest", "儲物箱", "箱子" }));
    set("no_get", 1);
    set("unit", "個");
    setup();
}

mixed *storage() {
    mixed *items;
    if (!home_owner) return ({});
    items = home_owner->query("custom_home/storage");
    if (!pointerp(items)) return ({});
    return items;
}

string long(int raw) {
    mixed *items;
    string s;
    int i;

    items = storage();
    s = sprintf("這是一只只屬於你的私人儲物箱。容量：%d / 200 格，不受重量限制。\n",
        sizeof(items));
    if (!sizeof(items))
        return s + "目前箱子裡空空如也。\n" +
            HIG "存放：store <物品>　取出：retrieve <編號>\n" NOR;

    s += "箱內物品：\n";
    for (i = 0; i < sizeof(items); i++)
        s += sprintf("  %3d. %s\n", i + 1, items[i]["name"]);
    s += HIG "存放：store <物品>　取出：retrieve <編號>\n" NOR;
    return s;
}

void init() {
    add_action("do_store", "store");
    add_action("do_store", "存放");
    add_action("do_retrieve", "retrieve");
    add_action("do_retrieve", "取出");
}

mapping pack_object(object ob) {
    mapping data;
    mixed param;

    data = ([
        "file" : base_name(ob),
        "name" : ob->name(),
    ]);

    if (function_exists("query_amount", ob))
        data["amount"] = (int)ob->query_amount();

    if (function_exists("query_autoload", ob)) {
        param = ob->query_autoload();
        if (stringp(param)) data["autoload"] = param;
    }
    // 登出會丟掉、但要存進儲物箱時保留內容的物品（例如繡花布衣）。
    if (function_exists("query_store_data", ob)) {
        param = ob->query_store_data();
        if (stringp(param)) data["store"] = param;
    }
    // 有附加屬性機制的武器（長劍、短劍、單刀、鬼頭刀）：連「普通」也記下來，
    // 取出時照原樣還原，避免存進去再拿出來就重新擲骰。
    if (stringp(ob->query("affix_base_name")))
        data["affix"] = mapp(ob->query("affix")) ? copy(ob->query("affix")) : ([]);
    return data;
}

int do_store(string arg) {
    object me, ob;
    mixed *items;

    me = this_player();
    if (!me || me != home_owner)
        return notify_fail("這不是你的私人儲物箱。\n");
    if (!arg || arg == "")
        return notify_fail("指令：store <物品>\n");

    items = storage();
    if (sizeof(items) >= 200)
        return notify_fail("私人儲物箱已經放滿 200 格。\n");

    ob = present(arg, me);
    if (!ob) return notify_fail("你身上沒有這件物品。\n");
    if (ob->is_character()) return notify_fail("活物不能塞進私人儲物箱。\n");
    if (ob->query("equipped")) return notify_fail("請先卸下這件裝備。\n");

    items += ({ pack_object(ob) });
    me->set("custom_home/storage", items);
    me->save();
    message_vision("$N把$n收進私人儲物箱。\n", me, ob);
    destruct(ob);
    return 1;
}

int do_retrieve(string arg) {
    object me, ob;
    mixed *items;
    mapping data;
    int n;

    me = this_player();
    if (!me || me != home_owner)
        return notify_fail("這不是你的私人儲物箱。\n");
    if (!arg || sscanf(arg, "%d", n) != 1)
        return notify_fail("指令：retrieve <編號>，先 look chest 查看編號。\n");

    items = storage();
    if (n < 1 || n > sizeof(items))
        return notify_fail("沒有這個儲物格。\n");

    data = items[n - 1];
    ob = new(data["file"]);
    if (!ob) return notify_fail("這件物品暫時無法從儲物箱取出。\n");

    if (!undefinedp(data["amount"]) && function_exists("set_amount", ob))
        ob->set_amount(data["amount"]);
    if (!undefinedp(data["autoload"]) && function_exists("autoload", ob))
        ob->autoload(data["autoload"]);
    if (!undefinedp(data["store"]) && function_exists("restore_store_data", ob))
        ob->restore_store_data(data["store"]);
    if (mapp(data["affix"]))
        ENHANCE_D->apply_affix(ob, data["affix"]);

    if (!ob->move(me)) {
        destruct(ob);
        return notify_fail("你目前拿不下這件物品。\n");
    }

    items[n - 1] = 0;
    items -= ({ 0 });
    me->set("custom_home/storage", items);
    me->save();
    tell_object(me, "你從私人儲物箱取出了" + ob->name() + "。\n");
    return 1;
}
