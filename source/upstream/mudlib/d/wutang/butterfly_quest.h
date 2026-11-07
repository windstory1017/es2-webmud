// /d/wutang/butterfly_quest.h — 蝶影幻步任務（葉翔任務）共用的記號與小工具。
// 設計與對白見 docs/martial_arts/瑯夷派_蝶影幻步.md「取得方式：葉翔任務」。
//
// 玩家身上的記號（用 set，重新上線仍有效）：
//   BQ_STAGE：任務進度，0 還沒開始，見下方 BQ_*。
//   BQ_MEET ：在後院和老人碰面了幾次（只在進度 0 時計算）。
// 道具（金牌、小紅巾、飲血）記著是誰取得的（quest_owner），不是本人交給 NPC 就不算，NPC 不收。
// 每個玩家各自解各自的任務：進度存在玩家身上，NPC 不鎖定任何人。

#include <ansi.h>

#define BQ_STAGE    "wutang/butterfly/stage"
#define BQ_MEET     "wutang/butterfly/meet"
#define BQ_TICK     2                       // 一個 tick 的秒數

#define BQ_NONE     0                       // 還沒開始（碰面中）
#define BQ_ASKED    1                       // 老人已開口請託，等玩家回應
#define BQ_ACCEPTED 2                       // 接受了，要去暗巷找李山
#define BQ_LEE      3                       // 李山說出藏金牌的地方，可以去搜小溪
#define BQ_PLAQUE   4                       // 拿到金牌，要交給老人
#define BQ_SCARF    5                       // 拿到小紅巾，要交給廟祝
#define BQ_BLADE    6                       // 拿到飲血，要交給葉翔

#define BQ_PLAQUE_FILE "/d/wutang/obj/golden_plaque"
#define BQ_SCARF_FILE  "/d/wutang/obj/red_scarf"
#define BQ_BLADE_FILE  "/d/wutang/obj/drinkblood"

private int bq_stage(object who) { return who->query(BQ_STAGE); }

// 玩家身上有沒有「自己取得的」任務道具。
private object bq_find(object who, string file)
{
    foreach(object ob in all_inventory(who))
        if( base_name(ob) == file && ob->query("quest_owner") == getuid(who) ) return ob;
    return 0;
}

// 造一件記著主人的任務道具交給玩家；拿不動就放在地上。
private object bq_give(object who, string file)
{
    object ob = new(file);

    ob->set("quest_owner", getuid(who));
    if( !ob->move(who) ) ob->move(environment(who));
    return ob;
}

// 是自己取得的任務道具才收（別人轉手的不算）。
private int bq_mine(object who, object ob, string file)
{
    return base_name(ob) == file && ob->query("quest_owner") == getuid(who);
}

// NPC 對話：$N 是 NPC，$n 是玩家；青色（NPC 互動敘述）。
private void bq_say(object npc, object who, string msg)
{
    message_vision(CYN + msg + "\n" NOR, npc, who);
}

// 玩家說話是不是肯定的回應：「好」「可以」「是」「嗯」之類，不含「不」「沒有」；
// 單純問候的「你好」「您好」不算。
private int bq_positive(string arg)
{
    if( !stringp(arg) ) return 0;
    arg = replace_string(replace_string(arg, "你好", ""), "您好", "");
    if( strsrch(arg, "不") != -1 || strsrch(arg, "沒有") != -1 ) return 0;
    foreach(string w in ({ "好", "可以", "是", "行", "嗯", "恩", "沒問題", "願意", "yes", "Yes", "ok", "OK" }))
        if( strsrch(arg, w) != -1 ) return 1;
    return 0;
}
