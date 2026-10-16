/*---
description: 龍圖 HERB 技能資格判定。三種交易共用；六句訊息沿用 HERB 0.1.0 原文。
note: 先判讀書識字，不足即只顯示第一句並結束（2026-10-14 追問 8）。
      只取原生技能等級 query_skill(skill, 1)。
---*/

void create() { seteuid(getuid()); }

// 自己看到的訊息依性別顯示你／妳（2026-10-15 使用者決定）。
private int deny(object me, string text) { tell_object(me, replace_string(text, "你", gender_self(me)) + "\n"); return 0; }

// req_ob：提供 requirement 的成品藍本；name：訊息中顯示的丹藥名稱。
int check(object me, object req_ob, string name, string mode)
{
    mapping req;
    if (!me || !req_ob) return 0;
    if (mode != "recipe" && mode != "buy" && mode != "sell") return 0;
    req = req_ob->query("requirement");
    if (!mapp(req) || !intp(req["literate"]) || req["literate"] < 1)
        return deny(me, "這項商品的技能需求資料不完整，暫時無法交易。");
    if (!undefinedp(req["alchemy-medication"])
    && (!intp(req["alchemy-medication"]) || req["alchemy-medication"] < 1))
        return deny(me, "這項商品的技能需求資料不完整，暫時無法交易。");

    if (me->query_skill("literate", 1) < req["literate"]) {
        if (mode == "recipe") return deny(me, "你書讀得還不夠多，根本不懂這個配方。");
        if (mode == "buy") return deny(me, "你書讀得還不夠多，根本不懂這顆丹藥如何辨識真偽。");
        return deny(me, sprintf("你書讀得還不夠多，根本不敢貿然拿出%s來換取銀兩。", name));
    }
    if (!undefinedp(req["alchemy-medication"])
    && me->query_skill("alchemy-medication", 1) < req["alchemy-medication"]) {
        if (mode == "recipe") return deny(me, sprintf("你在神農之術上的造詣不夠，不曉得如何煉製%s。", name));
        if (mode == "buy") return deny(me, sprintf("你在神農之術上的造詣不夠，不曉得如何辨識%s。", name));
        return deny(me, sprintf("你在神農之術上的造詣不夠，根本不敢假裝%s是你所煉製。", name));
    }
    return 1;
}

// herblist 用：不輸出訊息，回傳 0 可購買、1 讀書識字不足、2 神農之術不足、3 兩者皆不足、-1 資料不完整。
int status(object me, object req_ob)
{
    mapping req;
    int lit, med;
    if (!me || !req_ob) return -1;
    req = req_ob->query("requirement");
    if (!mapp(req) || !intp(req["literate"]) || req["literate"] < 1) return -1;
    lit = me->query_skill("literate", 1) < req["literate"];
    med = !undefinedp(req["alchemy-medication"])
        && me->query_skill("alchemy-medication", 1) < req["alchemy-medication"];
    return (lit ? 1 : 0) + (med ? 2 : 0);
}
