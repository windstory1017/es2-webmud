/*---
description: 龍圖 HERB 交易核心（HERB 房間專用）。以 HERB 0.1.0 herb_d.c 為底改寫，
             依 2026-10-14／15 決議：
             - 方士限定；非 HERB 指令一律 return 0 交回原版。
             - 只從銀行存款 bank_account 扣款／入帳。
             - 第一層同檔案成品／藥方拒購；單一堆疊、一次原生 move() 交付。
             - 額度不足才縮減；存款、負重不足整筆拒絕；負重交由原生 move() 判定。
             - 週期見 herb_cycle.c；資格見 herb_qualification.c。
             - 買成藥、買藥方、賣丹成交後都不忙碌（2026-10-24 取消原本的 start_busy(2)）。
             - 失敗回復；回復失敗則以暫時標記鎖定該玩家的 HERB 交易。
---*/

#include <command.h>

#define CATALOG  "/daemon/misc/herb_catalog"
#define QUALIFY  "/daemon/misc/herb_qualification"
#define CYCLE    "/daemon/misc/herb_cycle"
#define MARK     "longtu_herb/purchase"
#define BANK     "bank_account"
#define BANK_MAX 2000000000

void create() { seteuid(getuid()); }

/* ------------------------------------------------------------------ 工具 */

#define RECOVERY_LOG "/data/daemon/herb_recovery.log"

// 回復失敗紀錄（MUDLIB_UID 可寫 /data/daemon；/log 不可寫）。
private void record(string text)
{
    assure_file(RECOVERY_LOG);
    write_file(RECOVERY_LOG, text);
}

// 自己看到的訊息依性別顯示你／妳（2026-10-15 使用者決定）。
private int say_to(object me, string text) { tell_object(me, replace_string(text, "你", gender_self(me)) + "\n"); return 1; }

private int is_alchemist(object me)
{
    return objectp(me) && userp(me) && me->query_class() == "alchemist";
}

private string money(int amount) { return BALANCE_CMD->money_string(amount); }

private string unit_of(object ob)
{
    string u;
    u = ob ? ob->query("base_unit") : 0;
    return stringp(u) ? u : "粒";
}

// 第一層是否已有同一檔案的物件。
private int holds_file(object me, string file)
{
    object ob;
    foreach (ob in all_inventory(me))
        if (base_name(ob) == file) return 1;
    return 0;
}

// 共同前置檢查；不通過時已輸出訊息並回傳 0。
private int ready(object me)
{
    if (me->query_temp("herb/blocked"))
        return !say_to(me, "這筆交易的回復尚未確認，請通知管理員處理。");
    if (me->query_temp("herb/lock"))
        return !say_to(me, "你的上一筆交易尚未完成。");
    if (me->is_busy() || me->is_fighting())
        return !say_to(me, "你現在沒有空交易。");
    return 1;
}

private int bank_ok(object me)
{
    mixed bank;
    bank = me->query(BANK);
    if (bank && (!intp(bank) || bank < 0))
        return !say_to(me, "你的銀行資料異常，請通知管理員。");
    return 1;
}

/* ------------------------------------------------------------ herblist */

int cmd_herblist(object me, string arg)
{
    string key, *rows, msg, *order;
    mapping groups;
    object pill;
    int st;

    if (!is_alchemist(me)) return 0;

    rows = ({});
    groups = ([ 1: ({}), 2: ({}), 3: ({}) ]);
    foreach (key in CATALOG->query_keys()) {
        pill = CATALOG->product(key);
        if (!pill) continue;
        st = QUALIFY->status(me, pill);
        if (st < 0) continue;
        if (st == 0)
            rows += ({ cjk_pad(pill->name(), 14) + cjk_pad(money(CATALOG->price(key, "recipe") * CATALOG->recipe_count(key)), 24)
                       + money(CATALOG->price(key, "buy")) });
        else
            groups[st] += ({ pill->name() });
    }

    if (!sizeof(rows)) return say_to(me, "你目前還無法購買任何丹藥。");

    msg = cjk_pad("丹藥名稱", 14) + cjk_pad("配方", 24) + "成丹\n";
    msg += "------------------------------------------------------------\n";
    msg += implode(rows, "\n") + "\n";
    if (sizeof(groups[1])) msg += "※ " + implode(groups[1], "、") + "：讀書識字不足，無法購買。\n";
    if (sizeof(groups[2])) msg += "※ " + implode(groups[2], "、") + "：神農之術不足，無法購買。\n";
    if (sizeof(groups[3])) msg += "※ " + implode(groups[3], "、") + "：讀書識字與神農之術皆不足，無法購買。\n";
    me->start_more_if_needed(msg);
    return 1;
}

/* ------------------------------------------------------------ 交易執行 */

// tx 欄位：player mode key count price file object total old_bank old_mark new_mark
private void apply_tx(mapping tx)
{
    object me, ob;
    int saved;
    mixed err;

    me = tx["player"];
    if (tx["mode"] == "sell") {
        ob = tx["object"];
        if (tx["count"] < tx["total"]) {
            tx["qty_dirty"] = 1;
            ob->set_amount(tx["total"] - tx["count"]);
        }
        // 整堆賣出：存檔成功後才銷毀（成品不在存檔內，失敗時原物仍在身上）。
        tx["bank_dirty"] = 1;
        me->set(BANK, tx["old_bank"] + tx["price"] * tx["count"]);
    } else {
        ob = new(tx["file"]);
        tx["object"] = ob;
        ob->set_amount(tx["count"]);
        if (!ob->move(me)) {          // 負重、欄位交由原生 move() 判定
            destruct(ob);
            tx["object"] = 0;
            tx["no_room"] = 1;
            return;
        }
        tx["bank_dirty"] = 1;
        me->set(BANK, tx["old_bank"] - tx["price"] * tx["count"]);
        if (tx["mode"] == "buy") {
            tx["mark_dirty"] = 1;
            me->set(MARK, tx["new_mark"]);
        }
    }
    tx["save_attempted"] = 1;
    err = catch(saved = me->save());
    if (err || !saved) error("HERB player save failed.\n");
    tx["committed"] = 1;
}

private int rollback(mapping tx)
{
    object me, ob;
    int saved;
    mixed err;

    me = tx["player"];
    if (!me) return 0;
    if (tx["mark_dirty"]) {
        if (tx["old_mark"]) me->set(MARK, tx["old_mark"]);
        else me->delete(MARK);
    }
    if (tx["bank_dirty"]) me->set(BANK, tx["old_bank"]);
    ob = tx["object"];
    if (tx["mode"] == "sell") {
        if (tx["qty_dirty"]) {
            if (!ob) return 0;
            ob->set_amount(tx["total"]);
        }
    } else if (ob) destruct(ob);
    if (tx["save_attempted"]) {
        err = catch(saved = me->save());
        if (err || !saved) return 0;
    }
    return 1;
}

// 回傳 1：成交；0：未成交（訊息已輸出）。
private int run(mapping tx)
{
    object me;
    mixed err, rerr;
    int ok;

    me = tx["player"];
    me->set_temp("herb/lock", 1);
    err = catch(apply_tx(tx));
    if (!err && tx["no_room"]) {
        me->delete_temp("herb/lock");
        say_to(me, tx["mode"] == "recipe" ? "你身上已經放不下這批配方了。" : "你身上已經放不下這些丹藥了。");
        return 0;
    }
    if (err && !tx["committed"]) {
        rerr = catch(ok = rollback(tx));
        me->delete_temp("herb/lock");
        if (rerr || !ok) {
            me->set_temp("herb/blocked", 1);
            me->set_temp("herb/recovery", tx);
            catch(record(sprintf("[%s] %s recovery failed: %O tx=%O\n", ctime(time()), getuid(me), err, tx)));
            say_to(me, "交易未成立，但回復未能完成，請立即通知管理員。");
            return 0;
        }
        say_to(me, "交易未成立，存款、物品與購買紀錄已回復。");
        return 0;
    }
    if (tx["mode"] == "sell" && tx["count"] == tx["total"] && tx["object"]) {
        if (catch(destruct(tx["object"]))) {
            me->delete_temp("herb/lock");
            say_to(me, "交易已存檔，物品整理發生異常，請通知管理員。");
            return 1;
        }
    }
    me->delete_temp("herb/lock");
    return 1;
}

/* --------------------------------------------------------------- buy */

private int buy_product(object me, string key, int want)
{
    object pill;
    string file, name;
    mixed mark;
    int level, left, count, price, bank, cut, v;

    if (!ready(me)) return 1;
    pill = CATALOG->product(key);
    if (!pill) return say_to(me, "這項商品的技能需求資料不完整，暫時無法交易。");
    name = pill->name();
    if (!QUALIFY->check(me, pill, name, "buy")) return 1;

    file = CATALOG->product_path(key);
    if (holds_file(me, file)) return say_to(me, "你身上已有" + name + "，無法再購買。");

    mark = me->query(MARK, 1);
    v = CYCLE->valid(mark);
    if (v < 0) return say_to(me, "你的購買紀錄格式異常，請通知管理員。");

    level = me->query_skill("alchemy-medication", 1);       // 神農之術原生等級
    left = CATALOG->buy_limit(key, level) - CYCLE->used(mark, key);   // 上限見 herb_catalog（P1／R10）
    if (left < 1) return say_to(me, "你的" + name + "已購買到你暫時所能購買的上限。");
    count = want;
    if (count > left) { count = left; cut = 1; }

    price = CATALOG->price(key, "buy");
    if (price < 1 || count > BANK_MAX / price) return say_to(me, "這筆交易的價格或數量無法處理。");
    if (!bank_ok(me)) return 1;
    bank = me->query(BANK);
    if (bank < 1) return say_to(me, "你在錢莊沒有存款，無法購買。");
    if (bank < price * count) return say_to(me, "你的存款不足以購買這些丹藥。");

    if (!run(([ "player": me, "mode": "buy", "key": key, "count": count, "price": price,
                "file": file, "old_bank": bank, "old_mark": mark,
                "new_mark": CYCLE->next_mark(mark, key, count) ])))
        return 1;

    if (cut)
        return say_to(me, sprintf("你的%s只剩%s%s可購買，你向藥鋪買下%s%s%s，已從錢莊扣款。",
            name, chinese_number(count), unit_of(pill), chinese_number(count), unit_of(pill), name));
    return say_to(me, sprintf("你向藥鋪買下%s%s%s，已從錢莊扣款。", chinese_number(count), unit_of(pill), name));
}

private int buy_recipe(object me, string key)
{
    object pill;
    string file, name;
    int price, bank, count;

    if (!ready(me)) return 1;
    pill = CATALOG->product(key);
    if (!pill) return say_to(me, "這項商品的技能需求資料不完整，暫時無法交易。");
    name = pill->name();
    if (!QUALIFY->check(me, pill, name, "recipe")) return 1;

    file = CATALOG->mixture_path(key);
    if (holds_file(me, file)) return say_to(me, "你身上已有" + name + "配方，無法再購買。");

    price = CATALOG->price(key, "recipe");
    if (price < 1) return say_to(me, "這筆交易的價格或數量無法處理。");
    if (!bank_ok(me)) return 1;
    bank = me->query(BANK);
    if (bank < 1) return say_to(me, "你在錢莊沒有存款，無法購買。");
    count = CATALOG->recipe_count(key);
    if (bank < price * count) return say_to(me, "你的存款不足以購買這批配方。");

    if (!run(([ "player": me, "mode": "recipe", "key": key, "count": count, "price": price,
                "file": file, "old_bank": bank ])))
        return 1;
    return say_to(me, "你向藥鋪買下" + chinese_number(count) + "份" + name + "配方。");     // 配方不進入忙碌
}

int cmd_buy(object me, string arg)
{
    string id, key;
    int n;

    if (!is_alchemist(me) || !stringp(arg)) return 0;
    if (strsrch(arg, " from ") >= 0) return 0;              // 原版 NPC 購買

    if (sscanf(arg, "%d mixture for %s", n, id) == 2) {
        if (!(key = CATALOG->resolve(id))) return 0;
        return say_to(me, "購買配方請使用 buy mixture for " + id);
    }
    if (sscanf(arg, "mixture for %s", id) == 1) {
        if (!(key = CATALOG->resolve(id))) return 0;
        return buy_recipe(me, key);
    }
    if (sscanf(arg, "%d %s", n, id) == 2) {
        if (!(key = CATALOG->resolve(id))) return 0;
        if (n < 1) return say_to(me, "交易數量必須是正整數。");
        return buy_product(me, key, n);
    }
    if (key = CATALOG->resolve(arg)) return buy_product(me, key, 1);
    return 0;
}

/* -------------------------------------------------------------- sell */

int cmd_sell(object me, string arg)
{
    object ob, pill;
    string name, key, label;
    int n, has_n, total, count, price, bank;

    if (!is_alchemist(me) || !stringp(arg)) return 0;
    if (sscanf(arg, "%d %s", n, name) == 2) has_n = 1;
    else name = arg;

    ob = present(name, me);                                 // 只找第一層的第一個
    if (!ob || environment(ob) != me) return 0;
    if (!ob->id("pill")) return 0;                          // 非 pill（含藥方）交回原版

    label = ob->name();
    if (!(key = CATALOG->sell_key(ob)))
        return say_to(me, "你所持有的" + label + "是不收購的。");
    if (has_n && n < 1) return say_to(me, "交易數量必須是正整數。");

    if (!ready(me)) return 1;
    pill = CATALOG->product(key);                           // 技能需求一律讀方士版成品
    if (!pill) return say_to(me, "這項商品的技能需求資料不完整，暫時無法交易。");
    if (!QUALIFY->check(me, pill, label, "sell")) return 1;

    total = ob->query_amount();
    count = has_n ? n : total;
    if (count > total) return say_to(me, "你身上的" + label + "沒有那麼多。");

    price = CATALOG->price(key, "sell");
    if (price < 1 || count > BANK_MAX / price) return say_to(me, "這筆交易的價格或數量無法處理。");
    if (!bank_ok(me)) return 1;
    bank = me->query(BANK);
    if (bank > BANK_MAX - price * count) return say_to(me, "這筆交易會超過銀行可處理的金額。");

    if (!run(([ "player": me, "mode": "sell", "key": key, "count": count, "price": price,
                "object": ob, "total": total, "old_bank": bank ])))
        return 1;

    return say_to(me, sprintf("你將%s%s%s賣給藥鋪，得到%s，已存入錢莊。",
        chinese_number(count), unit_of(pill), label, money(price * count)));
}
