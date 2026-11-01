/*---
description: 羿水擺渡的船夫共用檔（老駱、小黃），五堂鎮河邊 ↔ 紫煙鎮渡口。
rule: 使用者指示（2026-10-30），設計見 docs/AREA_SWAYLAN.md「船夫擺渡」：
      - 給船夫總值 50 文以上的錢（多給不找），一趟只載付錢的那一位；
      - 玩家與船夫上這位船夫自己的「渡船上」，5 tick 後一起靠到對岸，船夫留在對岸；
      - 老駱、小黃同在一岸時，3 tick 後小黃自己撐船到對岸（見 huang.c）。
note: 不可 #include <npc.h>：該檔會再繼承一次 /std/char/npc.c，與 NPC 本身重複。
---*/

#include <dbase.h>
#include <name.h>

#define FARE        50          // 船資（文）
#define TICK        2           // 一個 tick 的秒數
#define CROSS_TICKS 5           // 過河要幾個 tick

string *BANKS = ({ "/d/wutang/riverside", "/d/ziyan/ferry" });

int crossing;                   // 正在河上（載客或換岸）

int is_crossing() { return crossing; }

// 這位船夫自己的「渡船上」房間，由各船夫檔設定 set("boat_room", ...)。
string boat_room() { return query("boat_room"); }

// 目前所在岸邊的對岸；不在岸邊時傳回 0。
string other_bank()
{
    object env = environment();
    int i;

    if( !env ) return 0;
    i = member_array(base_name(env), BANKS);
    if( i < 0 ) return 0;
    return BANKS[1 - i];
}

int accept_object(object who, object ob)
{
    if( crossing || !other_bank() ) return 0;
    if( !ob->query("money_id") ) return 0;
    if( ob->value() < FARE ) {
        command("say 過河要五十文錢，這點不夠哪。");
        return 0;
    }
    crossing = 1;
    // 等 give 的訊息先出來，再上船。
    call_out("depart", 0, who, other_bank());
    return 1;
}

void depart(object who, string dest)
{
    object boat;

    if( !objectp(who) || environment(who) != environment() ) {
        crossing = 0;
        return;
    }
    boat = load_object(boat_room());
    message_vision("$N收下錢，笑道：「客倌請上船，坐穩嘍！」\n", this_object());
    message_vision("$N領著$n跳上小船，竹篙一點，小船便離了岸。\n", this_object(), who);
    this_object()->move(boat);
    who->move(boat);
    call_out("arrive", CROSS_TICKS * TICK, who, dest);
}

void arrive(object who, string dest)
{
    object bank = load_object(dest);

    // 先在船上說靠岸，再一起上岸，乘客才不會先看到岸上的景色。
    if( objectp(who) && environment(who) == load_object(boat_room()) ) {
        message_vision("小船靠了岸，$N把纜繩繫在木樁上，說道：「到嘍，客倌慢走。」\n", this_object());
        this_object()->move(bank);
        who->move(bank);
    } else {
        this_object()->move(bank);
        message_vision("一條小船靠了岸，$N把纜繩繫在木樁上，跳上岸來。\n", this_object());
    }
    crossing = 0;
}

// 不載客，自己撐船到對岸（小黃換岸用）。
void cross_alone(string leave_msg, string arrive_msg)
{
    string dest = other_bank();

    if( crossing || !dest ) return;
    crossing = 1;
    message_vision(leave_msg, this_object());
    this_object()->move(load_object(boat_room()));
    call_out("arrive_alone", CROSS_TICKS * TICK, dest, arrive_msg);
}

void arrive_alone(string dest, string arrive_msg)
{
    this_object()->move(load_object(dest));
    message_vision(arrive_msg, this_object());
    crossing = 0;
}

