/* CUSTOM WEBMUD HUD BRIDGE
 * Read-only status/inventory feed for the browser context panel. Not canonical gameplay.
 * Buttons in the browser only submit canonical ES2 commands back to the server.
 */
inherit F_CLEAN_UP;

// MODIFIED: added missing create()/seteuid. Every other command file sets an
// effective uid this way; webhud.c didn't, so its load_object() call below
// (resolving an exit's destination room name) always failed with "Can't load
// objects when no effective user" — caught and swallowed, but it delayed/
// corrupted this HUD emission often enough that the client's silent-poll
// timing got out of sync, leaking raw @@WEBHUD|...| lines into the visible
// terminal text.
private void create() { seteuid(getuid()); }

private string field(string s)
{
    if (!stringp(s)) return "";
    s = replace_string(s, "|", "/");
    s = replace_string(s, "\n", " ");
    s = replace_string(s, "\r", " ");
    return s;
}

private void emit_side(string side, object ob)
{
    // 最後四欄是形體／精／氣／神的最大值（score 顯示的分母），加在尾端以相容舊版網頁。
    printf("@@WEBHUD|%s|%s|STAT|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
        side, field(ob->name()),
        ob->query_stat("HP"), ob->query_stat_effective("HP"),
        ob->query_stat("gin"), ob->query_stat_effective("gin"),
        ob->query_stat("kee"), ob->query_stat_effective("kee"),
        ob->query_stat("sen"), ob->query_stat_effective("sen"),
        ob->query_level(),
        ob->query_stat("food"), ob->query_stat_maximum("food"),
        ob->query_stat("water"), ob->query_stat_maximum("water"),
        ob->query_stat("fatigue"), ob->query_stat_maximum("fatigue"),
        ob->query_stat_maximum("HP"), ob->query_stat_maximum("gin"),
        ob->query_stat_maximum("kee"), ob->query_stat_maximum("sen"));
}

private string item_action(object ob)
{
    object liq;

    if (function_exists("stuff_ob", ob)) return "eat";
    if (ob->query("liquid_container")) {
        foreach (liq in all_inventory(ob))
            if (function_exists("drink_ob", liq) && liq->query_volume()) return "drink";
    }
    if (function_exists("study_ob", ob)) return "study";
    return "none";
}

private string item_command(object ob, string action)
{
    string id;
    id = ob->query_id();
    if (!stringp(id) || id == "") id = ob->query("id");
    if (!stringp(id) || id == "") return "";
    if (action == "eat") return "eat " + id;
    if (action == "drink") return "drink from " + id;
    if (action == "study") return "study " + id;
    return "";
}

private void emit_inventory(object me)
{
    object ob;
    string action, command, name, id;

    write("@@WEBHUD|INVBEGIN\n");
    foreach (ob in all_inventory(me)) {
        action = item_action(ob);
        command = item_command(ob, action);
        name = ob->short();
        if (!stringp(name) || name == "") name = ob->name();
        id = ob->query_id();
        if (!stringp(id)) id = "";
        if (action == "none" || command == "") continue;
        printf("@@WEBHUD|ITEM|%s|%s|%s|%s|%d\n",
            field(name), field(id), action, field(command), ob->query("equipped") ? 1 : 0);
    }
    write("@@WEBHUD|INVEND\n");
}

int main(object me, string arg)
{
    object target;
    if (!me || !userp(me)) return 0;

    // 會送 webhud 的就是網頁玩家：記下來，等待輸入時才會通知網頁暫停查詢（見 feature/user/input.c）。
    // 連線物件也記，魂飛魄散後重新選種族時還用得到。
    me->set_temp("webhud/client", 1);
    if (objectp(me->link())) me->link()->set_temp("webhud/client", 1);

    write("@@WEBHUD|BEGIN\n");
    if (environment(me)) {
        mapping exits;
        string dir, dest;
        printf("@@WEBHUD|ROOM|%s|%s\n", field(base_name(environment(me))), field(environment(me)->query("short")));
        /* Optional, generic map presentation metadata. Rooms that do not set
         * these remain ordinary ground-level rooms. This does not alter exits
         * or gameplay; it only tells the browser how much topology to reveal. */
        printf("@@WEBHUD|MAPMETA|%s|%s|%s|%s\n",
            field(base_name(environment(me))),
            field(environment(me)->query("map/area")),
            field(environment(me)->query("map/layer")),
            field(environment(me)->query("map/mode")));
        exits = environment(me)->query("exits");
        if (mapp(exits)) {
            foreach (dir, dest in exits) {
                if (!stringp(dir) || !stringp(dest)) continue;
                object dest_ob;
                string dest_name;
                dest_ob = find_object(dest);
                if (!dest_ob) catch(dest_ob = load_object(dest));
                dest_name = dest_ob ? dest_ob->query("short") : "";
                printf("@@WEBHUD|EXIT|%s|%s|%s|%s\n",
                    field(base_name(environment(me))), field(dir), field(dest), field(dest_name));
            }
        }
    }
    emit_inventory(me);
    emit_side("SELF", me);

    target = me->query_opponent();
    if (!target || !me->is_fighting(target)) {
        write("@@WEBHUD|NONE|idle\n");
        write("@@WEBHUD|END\n");
        return 1;
    }
    emit_side("TARGET", target);
    write("@@WEBHUD|END\n");
    return 1;
}

int help(object me) { return 1; }
