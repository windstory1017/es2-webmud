/* 武陀灸堂針法效果的共用基底（不是獨立的 condition）。NEW
 *
 * 設計：docs/martial_arts/武陀灸堂_門派設計.md
 * 用法：子檔 inherit 本檔，定義 end_message(me, data) 顯示效果結束的敘述。
 * 施針時 set_condition(名稱, ([ "apply": ([ "armor": 50, ... ]), "until": 結束時間,
 *   "serial": 序號, ... ]))；apply 的每一項加到 apply/<項目>，結束時扣回。
 * 重新登入時由 condition_apply() 補回；時間到由 call_out 結束，
 * 心跳的 condition_update() 作為備援（比照 rain_blessing.c）。
 */
#include <condition.h>

inherit CONDITION;

void create() { seteuid(getuid()); }

// 結束靠 call_out，保持載入。
int clean_up() { return 1; }

void end_message(object me, mapping data) { }

private void finish(object me, string cnd)
{
    mapping data = me->query_condition(cnd);

    me->delete_condition(cnd);
    if( mapp(data) ) end_message(me, data);
}

void expire(object me, string cnd, int serial)
{
    mapping data;

    if( !objectp(me) ) return;
    data = me->query_condition(cnd);
    if( !mapp(data) || data["serial"] != serial ) return;
    finish(me, cnd);
}

void condition_apply(object me, string cnd, mixed data)
{
    int left;

    if( !mapp(data) || !mapp(data["apply"]) ) return;
    foreach(string key, int n in data["apply"])
        me->add_temp("apply/" + key, n);
    left = data["until"] - time();
    call_out("expire", left > 0 ? left : 1, me, cnd, data["serial"]);
}

// delete_condition() 呼叫時不帶 data，從身上的 condition 取。
varargs void condition_unapply(object me, string cnd, mixed data)
{
    if( !mapp(data) ) data = me->query_condition(cnd);
    if( !mapp(data) || !mapp(data["apply"]) ) return;
    foreach(string key, int n in data["apply"])
        me->add_temp("apply/" + key, -n);
}

void condition_update(object me, string cnd, mixed data)
{
    if( !mapp(data) ) me->delete_condition(cnd);
    else if( data["until"] <= time() ) finish(me, cnd);
}
