/*---
description: 龍圖 HERB 商品資料。八種成品的檔案、藥方檔、價格計算與收購範圍。
note: 價格一律由成品檔 base_value / mixture_value 計算，不另存價目表。
      技能需求一律讀成品檔 requirement，不在此重複。
---*/

#define PRODUCT_DIR "/obj/alchemist/"
#define MIXTURE_DIR "/obj/alchemist/"

private string *keys = ({ "black", "lavage", "green", "relax", "aqua",
                          "refresh", "mana", "minor", "major" });

private mapping product_file = ([
    "black":   "black_pill",
    "lavage":  "lavage_pill",
    "green":   "green_pill",
    "relax":   "relax_pill",
    "aqua":    "aquapill",
    "refresh": "refresh_pill",
    "mana":    "mana_pill",
    "minor":   "minor_heal_pill",
    "major":   "major_heal_pill",
]);

// 收購範圍：檔案路徑 -> 商品代碼。藥鋪賣的烏心丹也收（設計檔七）。
private mapping sell_files;

void create()
{
    string key;
    seteuid(getuid());
    sell_files = ([ ]);
    foreach (key in keys) sell_files[PRODUCT_DIR + product_file[key]] = key;
}

string *query_keys() { return keys + ({}); }

string product_path(string key)
{
    if (!stringp(key) || undefinedp(product_file[key])) return 0;
    return PRODUCT_DIR + product_file[key];
}

// 成丹每週期購買上限（P1，2026-10-16 使用者決定 R10）：
// 上限 ＝ 基數 ＋ 神農之術原生等級 ÷ 除數（無條件捨去）。新增丹藥時在此補一列。
private mapping buy_cap = ([
    "black":   ({ 20, 1 }),
    "lavage":  ({ 30, 2 }),
    "green":   ({ 30, 2 }),
    "relax":   ({ 20, 2 }),
    "aqua":    ({ 20, 2 }),
    "refresh": ({ 20, 3 }),
    "mana":    ({ 20, 3 }),
    "minor":   ({ 20, 3 }),
    "major":   ({ 5, 5 }),
]);

int buy_limit(string key, int skill)
{
    mixed *row;
    if (!stringp(key) || !arrayp(row = buy_cap[key])) return 0;
    if (skill < 0) skill = 0;
    return row[0] + skill / row[1];
}

// 藥鋪不販售的成丹：大還丹（2026-11-14 使用者決定，避免買來用黃白之術轉錢）。
// 收購與配方照舊。
int sells_product(string key) { return key != "major"; }

// 藥鋪一次賣幾份配方：大還丹一份，其餘五份（2026-10-15 使用者決定 R5）。
int recipe_count(string key) { return key == "major" ? 1 : 5; }

string mixture_path(string key)
{
    if (!product_path(key)) return 0;
    return MIXTURE_DIR + "mixture_" + key + "pill";
}

// 成品藍本（只讀資料用，不交付給玩家）。
object product(string key)
{
    string file;
    object ob;
    file = product_path(key);
    if (!file) return 0;
    if (catch(ob = load_object(file))) return 0;
    return ob;
}

// 玩家輸入的丹藥 ID -> 商品代碼。必須與成品 ID 完全相同；單獨的 pill 不算。
string resolve(string text)
{
    string key;
    object ob;
    if (!stringp(text) || text == "" || text == "pill") return 0;
    foreach (key in keys) {
        ob = product(key);
        if (ob && ob->id(text)) return key;
    }
    return 0;
}

// 物件檔案 -> 收購代碼；不在收購範圍回傳 0。
string sell_key(object ob)
{
    string file;
    if (!objectp(ob)) return 0;
    file = base_name(ob);
    if (undefinedp(sell_files[file])) return 0;
    return sell_files[file];
}

// 單價（文）。mode: "buy" 成品售價、"recipe" 一份配方、"sell" 收購價。
int price(string key, string mode)
{
    object ob;
    int base;
    ob = product(key);
    if (!ob) return -1;
    if (mode == "recipe") return ob->query("mixture_value");
    base = ob->query("base_value");
    if (base < 1 || base > 100000000) return -1;
    if (mode == "buy") return sells_product(key) ? base * 10 : -1;
    // 收購價 × 2（2026-11-07 由 × 5 調降：NPC 售價必須高於收購價，避免轉賣賺價差，見煉丹流程 T-38）。
    if (mode == "sell") return key == "black" ? base * 80 / 100 : base * 2;
    return -1;
}
