/*---
description: 販售龍圖丹藥的 NPC 商人共用檔（取代 F_VENDOR 使用）。
rule: NPC 向玩家販售丹藥時，依丹藥的階級（pill_grade）調整售價；
      目前二階丹藥以原價（base_value）三倍販售（使用者指示 #6，2026-10-23）。
      其他商品照原價。藥鋪櫃台（HERB）的價格另由 herb_catalog 計算，不受本檔影響。
note: 實付金額（query_trading_price）與商品清單（do_vendor_list）一律用同一個價格。
      要調整某一階的倍數，只改 GRADE_MARKUP。
---*/

#include <dbase.h>
#include <name.h>

// 不可 #include <npc.h>：該檔會再繼承一次 /std/char/npc.c，與 NPC 本身重複。
inherit "/feature/npc/vendor.c";

// 階級 -> 售價倍數。沒有列出的階級照原價。
private mapping GRADE_MARKUP = ([ 2 : 3 ]);

int query_trading_price(string handle)
{
    int v, rate;

    v = ::query_trading_price(handle);
    if( v < 1 ) return v;
    rate = GRADE_MARKUP[handle->query("pill_grade")];
    if( rate > 0 ) v *= rate;
    return v;
}

private string price_string(int v)
{
    if( v%10000 == 0 ) return chinese_number(v/10000) + "兩黃金";
    if( v%100 == 0 ) return chinese_number(v/100) + "兩銀子";
    return chinese_number(v) + "文錢";
}

// 與 F_VENDOR 的商品清單相同，只是價格改用 query_trading_price()。
int do_vendor_list(string arg)
{
    mapping goods;
    string list, item;
    int count;

    if( !mapp(goods = query("merchandise")) ) return 0;
    if( arg && !id(arg) ) return 0;

    list = "";
    foreach(item, count in goods) {
        if( count < 1 ) continue;
        list += "  " + cjk_pad(item->short(1), 30) + "： "
            + cjk_pad(price_string(query_trading_price(item)), 18, 1) + "\n";
    }
    if( list=="" ) {
        write( name() + "的貨物已經全部賣光了，下次早一點來吧！\n");
        return 1;
    }

    write("你可以購買下列這些東西：\n-------------------------\n" + list);
    return 1;
}
