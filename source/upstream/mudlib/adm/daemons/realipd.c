/*---
description: 記錄玩家真實 IP，並負責同 IP 在線人數限制。
---*/
// 玩家都經由網頁橋接程式從本機連入，遊戲看到的位址都是本機。橋接程式在每條連線一開始
// 送一行「@@REALIP <密碼> <真實IP> <同一戶代表>」，由登入程式交給這裡核對後記下。
// 以連線的來源埠號區分每一條連線；角色從登入物件換到身體時是同一條連線，不會跟丟。
// 密碼檔 /adm/etc/bridge_secret 由橋接程式產生，不放進 git。
//
// 同 IP 在線人數限制（iplimit 指令調整）：
//   limit  - 同一戶最多幾個玩家角色同時在世界裡，0 表示不限制
//   allow  - 白名單，同一戶代表 : 該戶的上限
// 斷線但身體還留在世界裡的角色也算；巫師不受限制也不計入。

#pragma save_binary

#include <ansi.h>

inherit F_SAVE;

#define SECRET_FILE	"/adm/etc/bridge_secret"
#define DEFAULT_LIMIT	3

int limit = DEFAULT_LIMIT;
mapping allow = ([]);

static mapping conn = ([]);	// 來源埠號 : ({ 真實IP, 同一戶代表 })
static string secret;

string query_save_file() { return DATA_DIR + "iplimit"; }

void create()
{
    seteuid(getuid());
    if (!restore()) limit = DEFAULT_LIMIT;
    if (!mapp(allow)) allow = ([]);
}

// 一直留在記憶體裡（連線紀錄不能被清掉）。
int clean_up() { return 0; }

private string port_of(object ob)
{
    string addr;
    int port;

    if (!objectp(ob) || !interactive(ob)) return 0;
    addr = socket_address(ob);
    if (!stringp(addr) || sscanf(addr, "%*s %d", port) != 2) return 0;
    return "" + port;
}

private string load_secret()
{
    string s = read_file(SECRET_FILE);

    if (!stringp(s)) return 0;
    s = replace_string(replace_string(s, "\n", ""), "\r", "");
    return strlen(s) >= 32 ? s : 0;
}

// 清掉已經斷線的連線紀錄。
private void prune()
{
    mapping alive = ([]);
    string key;

    foreach (object u in users())
        if (key = port_of(u)) alive[key] = 1;
    foreach (key in keys(conn))
        if (!alive[key]) map_delete(conn, key);
}

// 由登入程式呼叫：核對暗號並記下這條連線的真實 IP。
int register_line(object ob, string line)
{
    string pass, ip, group, key;

    if (previous_object() != find_object(LOGIN_D)) return 0;
    if (sscanf(line, "@@REALIP %s %s %s", pass, ip, group) != 3) return 0;
    if (!secret || pass != secret) secret = load_secret();
    if (!secret || pass != secret) return 0;
    if (!(key = port_of(ob))) return 0;
    prune();
    conn[key] = ({ ip, group });
    return 1;
}

string query_ip(object ob)
{
    string key = port_of(ob);
    return (key && conn[key]) ? conn[key][0] : 0;
}

string query_group(object ob)
{
    string key = port_of(ob);
    return (key && conn[key]) ? conn[key][1] : 0;
}

// 身體進入世界或斷線重連時記下 IP，斷線後仍能計算。
void mark_body(object body)
{
    string key;

    if (!objectp(body) || !(key = port_of(body)) || !conn[key]) return;
    body->set_temp("realip", conn[key][0]);
    body->set_temp("realip_group", conn[key][1]);
}

int query_limit() { return limit; }
mapping query_allow() { return copy(allow); }

int limit_of(string group)
{
    if (group && !undefinedp(allow[group])) return allow[group];
    return limit;
}

// 目前在世界裡（含斷線留在世界的）的玩家身體。
object *world_bodies()
{
    return filter_array(children(USER_OB),
        (: clonep($1) && environment($1) && $1->query("id") :));
}

// 某一戶目前在世界裡的玩家角色（不含巫師與 except）。
object *bodies_of(string group, object except)
{
    return filter_array(world_bodies(),
        (: $1 != $3 && !wizardp($1) && $1->query_temp("realip_group") == $2 :),
        group, except);
}

// 這條連線能不能再讓一個角色進入世界。
// ob：登入物件；id：角色代號；except：要排除的身體（重新連線回自己的身體時）。
int can_enter(object ob, string id, object except)
{
    string group;
    int n;

    if (wizhood(id) != "(player)") return 1;
    if (!(group = query_group(ob))) return 1;	// 沒有真實 IP（本機連線），不限制
    if ((n = limit_of(group)) <= 0) return 1;
    return sizeof(bodies_of(group, except)) < n;
}

// ---------- 以下供 iplimit 指令使用，只有 admin 能改 ----------

private int admin_ok()
{
    object me = this_player(1);
    return me && wizhood(me) == "(admin)";
}

private void log_change(string what)
{
    log_file("static/IPLIMIT", sprintf("[%s] %s %s\n",
        ctime(time()), this_player(1)->query("id"), what));
}

int set_limit(int n)
{
    if (!admin_ok() || n < 0) return 0;
    limit = n;
    save();
    log_change("set limit " + n);
    return 1;
}

int set_allow(string group, int n)
{
    if (!admin_ok() || !group || n < 0) return 0;
    allow[group] = n;
    save();
    log_change("allow " + group + " " + n);
    return 1;
}

int remove_allow(string group)
{
    if (!admin_ok() || undefinedp(allow[group])) return 0;
    map_delete(allow, group);
    save();
    log_change("remove " + group);
    return 1;
}
