/*---
description: 繡花布衣：鳳娘白甌（d/wutang/npc/bai_ou.c）繡過的布衣。
design: docs/EMBROIDERY.md（使用者指示，2026-11-13）
rule: - 由白甌產生新物件（不是原布衣改的）；產生時擲一次繡花結果（embroider()），八種互斥。
      - 記號 embroidered：繡過的衣服不能再繡。
      - 玩家可用 name 指令命名（custom_name），可重複命名。
      - 登出照一般規則丟在地上（使用者決定，2026-11-13）；不設 query_autoload。
        家園儲物箱存取用 query_store_data／restore_store_data，結果、屬性、詞綴、名字都保留。
---*/

#include <armor.h>

inherit F_CLOTH;

#define RESULT_ATTR      1   // 屬性
#define RESULT_SHARP     2   // 傷害力＋攻勢
#define RESULT_DULL      3   // 傷害力－攻勢
#define RESULT_GUARD     4   // 防禦力＋守勢
#define RESULT_LOOSE     5   // 守勢－
#define RESULT_AFFIX     6   // 詞綴
#define RESULT_MOVE      7   // 行動力
#define RESULT_AWARE     8   // 警覺性

#define DEFAULT_NAME "繡花布衣"
#define CLOTH_LONG "一件經名家之手繡過的布衣，雲紋在衣襟與袖口隱隱流轉。\n"

void create()
{
    set_name(DEFAULT_NAME, ({ "embroidered cloth", "cloth" }));
    set_weight(1000);
    setup_cloth();

    set("unit", "件");
    set("value", 200);
    set("wear_as", "cloth");
    set("embroidered", 1);
    set("apply_armor/cloth", ([ "armor": 1 ]));
    set("long", CLOTH_LONG);

    setup();
}

// 繡花：擲一次結果並套用（只在白甌產生時呼叫一次）。傳回結果代號，白甌依此多說一句。
int embroider()
{
    string *attrs = ({ "str", "cor", "cps", "int", "wis" });
    mapping a = ([ "armor": 1 ]);
    int r = random(100), result;

    if( r < 20 ) {
        a[attrs[random(sizeof(attrs))]] = 1 + random(3);
        result = RESULT_ATTR;
    } else if( r < 40 ) {
        a["damage"] = 1 + random(10);
        a["intimidate"] = 10 + random(11);
        result = RESULT_SHARP;
    } else if( r < 50 ) {
        a["damage"] = -(1 + random(10));
        a["intimidate"] = -(10 + random(11));
        result = RESULT_DULL;
    } else if( r < 70 ) {
        a["armor"] += 10 + random(6);
        a["wittiness"] = 10 + random(11);
        result = RESULT_GUARD;
    } else if( r < 80 ) {
        a["wittiness"] = -(10 + random(11));
        result = RESULT_LOOSE;
    } else if( r < 90 ) {
        result = RESULT_AFFIX;
    } else if( r < 95 ) {
        a["move"] = 25 + random(26);
        result = RESULT_MOVE;
    } else {
        a["awarness"] = 50 + random(51);
        result = RESULT_AWARE;
    }

    set("apply_armor/cloth", a);
    set("cloth_base_apply", copy(a));
    set("embroidery_result", result);
    if( result == RESULT_AFFIX ) ENHANCE_D->roll_cloth_affix(this_object());
    return result;
}

// 玩家命名（name 指令呼叫）：中文名稱換成玩家取的名字，英文名稱不變。
void set_custom_name(string str)
{
    set("custom_name", str);
    set("name", str);
}

/* ---- 家園儲物箱存取（登出不保留，所以不用 autoload） ------------------- */

// 格式："1|結果|屬性（k=v;k=v）|前綴|後綴|玩家取的名字"
string query_store_data()
{
    mapping base = query("cloth_base_apply"), affix = query("affix");
    string k, stats = "", prefix = "", suffix = "", cname = query("custom_name");
    int v;

    if( !mapp(base) ) base = query("apply_armor/cloth");
    if( mapp(base) )
        foreach(k, v in base) stats += (stats == "" ? "" : ";") + k + "=" + v;
    if( mapp(affix) ) {
        if( stringp(affix["prefix"]) ) prefix = affix["prefix"];
        if( stringp(affix["suffix"]) ) suffix = affix["suffix"];
    }
    return sprintf("1|%d|%s|%s|%s|%s", query("embroidery_result"), stats, prefix, suffix,
        stringp(cname) ? cname : "");
}

void restore_store_data(string param)
{
    string *p, s, k;
    mapping base = ([]), affix = ([]);
    int v;

    if( !stringp(param) ) return;
    p = explode(param, "|");
    if( sizeof(p) < 3 ) return;
    sscanf(p[1], "%d", v);
    set("embroidery_result", v);
    foreach(s in explode(p[2], ";"))
        if( sscanf(s, "%s=%d", k, v) == 2 ) base[k] = v;
    if( !sizeof(base) ) base = ([ "armor": 1 ]);
    set("apply_armor/cloth", base);
    set("cloth_base_apply", copy(base));
    if( sizeof(p) > 3 && p[3] != "" ) {
        affix["prefix"] = p[3];
        if( sizeof(p) > 4 && p[4] != "" ) affix["suffix"] = p[4];
        ENHANCE_D->apply_cloth_affix(this_object(), affix);
    }
    if( sizeof(p) > 5 && p[5] != "" ) set_custom_name(p[5]);
}
