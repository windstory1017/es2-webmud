// 玩家都經由網頁橋接程式從本機連入，driver 看到的位址都是本機。
// 有真實 IP（見 /adm/daemons/realipd.c）就回傳真實 IP，封鎖 IP、防洗角色、
// 上線位址紀錄等原有功能因此不用逐一修改。需要連線本身位址的地方用 efun:: 版本。

varargs string query_ip_number(object ob)
{
    object d;
    string ip;

    if (!ob && !(ob = this_player())) return 0;
    // 只用已載入的 realipd：這裡可能在沒有 euid 的情況下被呼叫（例如 master 的 connect()），不能載入物件。
    if ((d = find_object(REALIP_D)) && (ip = d->query_ip(ob))) return ip;
    return efun::query_ip_number(ob);
}

varargs string query_ip_name(object ob)
{
    object d;
    string ip;

    if (!ob && !(ob = this_player())) return 0;
    // 只用已載入的 realipd：這裡可能在沒有 euid 的情況下被呼叫（例如 master 的 connect()），不能載入物件。
    if ((d = find_object(REALIP_D)) && (ip = d->query_ip(ob))) return ip;
    return efun::query_ip_name(ob);
}
