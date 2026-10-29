// iplimit.c - 同 IP 在線人數限制（見 /adm/daemons/realipd.c）

#include <ansi.h>
#include <command.h>

inherit F_CLEAN_UP;

private string who(object ob)
{
    string s = ob->name(1) + "(" + ob->query("id") + ")";

    if (wizardp(ob)) s += "[巫師]";
    if (!interactive(ob)) s += "[斷線]";
    return s;
}

private string group_of(object ob)
{
    string g = REALIP_D->query_body_group(ob);
    return g ? g : "(本機)";
}

private int players(object *obs)
{
    return sizeof(filter_array(obs, (: !wizardp($1) :)));
}

// all：列出所有人；否則只列出有兩個以上玩家角色的 IP。
private void show(object me, int all)
{
    mapping groups = ([]), allow = REALIP_D->query_allow();
    object *bodies = REALIP_D->world_bodies();
    string *keys, msg, g;
    int limit = REALIP_D->query_limit(), over = 0;

    foreach (object ob in bodies) {
        g = group_of(ob);
        if (!groups[g]) groups[g] = ({});
        groups[g] += ({ ob });
    }

    msg = "同 IP 在線上限：" + (limit ? limit + " 人" : "不限制") + "\n";
    if (sizeof(allow)) {
        msg += "白名單：\n";
        foreach (g in sort_array(keys(allow), 1))
            msg += sprintf("  %-26s %s\n", g, allow[g] ? allow[g] + " 人" : "不限制");
    }
    msg += "\n";

    keys = sort_array(keys(groups), (: players($(groups)[$2]) - players($(groups)[$1]) :));
    foreach (g in keys) {
        int n = players(groups[g]), cap = REALIP_D->limit_of(g);
        int is_over = g != "(本機)" && cap > 0 && n > cap;

        if (is_over) over++;
        if (!all && n < 2) continue;
        // sprintf 以位元組計算寬度：「(本機)」佔 8 位元組、畫面只佔 6 格，多補兩格才會對齊。
        msg += sprintf("  %s%-" + (g == "(本機)" ? 28 : 26) + "s %2d/%-4s %s%s\n" NOR,
            is_over ? HIR : "", g, n,
            g == "(本機)" ? "-" : (cap ? "" + cap : "不限"),
            implode(map_array(groups[g], (: who :)), " "),
            undefinedp(allow[g]) ? "" : " （白名單）");
    }
    msg += sprintf("\n共 %d 個角色在世界裡，來自 %d 個 IP；%d 個 IP 超過上限。\n",
        sizeof(bodies), sizeof(groups), over);
    if (!all) msg += "（只列出有兩個以上玩家角色的 IP；iplimit list 列出全部）\n";
    me->start_more_if_needed(msg);
}

int main(object me, string arg)
{
    string ip;
    int n;

    SECURED_ADMIN_COMMAND;

    if (!arg || arg == "") {
        show(me, 0);
        return 1;
    }
    if (arg == "list") {
        show(me, 1);
        return 1;
    }
    if (sscanf(arg, "allow %s %d", ip, n) == 2) {
        if (n < 0) return notify_fail("人數不能小於 0。\n");
        if (strsrch(ip, ":") >= 0 && ip[<5..] != "::/64")
            return notify_fail("IPv6 請用 iplimit list 顯示的同一戶寫法（結尾是 ::/64）。\n");
        REALIP_D->set_allow(ip, n);
        write("白名單：" + ip + " 上限 " + (n ? n + " 人" : "不限制") + "。\n");
        return 1;
    }
    if (sscanf(arg, "remove %s", ip) == 1) {
        if (!REALIP_D->remove_allow(ip)) return notify_fail("白名單裡沒有 " + ip + "。\n");
        write("已將 " + ip + " 移出白名單。\n");
        return 1;
    }
    if (sscanf(arg, "%d", n) == 1 && arg == "" + n) {
        if (n < 0) return notify_fail("人數不能小於 0。\n");
        REALIP_D->set_limit(n);
        write("同 IP 在線上限設為" + (n ? " " + n + " 人" : "不限制") + "。已在線上的角色不受影響。\n");
        return 1;
    }
    return notify_fail("指令格式錯誤，請看 help iplimit。\n");
}

int help(object me)
{
    write(@HELP
指令格式：
  iplimit                    顯示目前上限、白名單，以及有兩個以上角色在線的 IP
  iplimit list               列出所有在世界裡的角色（含斷線的），依 IP 分組
  iplimit <人數>             設定同一個 IP 最多幾個玩家角色同時在世界裡，0 表示不限制
  iplimit allow <IP> <人數>  白名單：指定某個 IP 的上限（0 表示不限制）
  iplimit remove <IP>        移出白名單

說明：
  - 斷線但身體還留在世界裡的角色也算；重新連線回自己的角色一定可以。
  - 巫師不受限制，也不計入人數。
  - 調低上限時，已經在線上的人不會被踢，只影響之後新進入的人。
  - IPv6 以同一戶（前 64 位元）合併計算，寫法如 2001:db8:1:2::/64。
  - 本機直接連線（本機測試）不受限制，顯示為「(本機)」。
  - 每次修改記錄在 /log/static/IPLIMIT。
HELP
    );
    return 1;
}
