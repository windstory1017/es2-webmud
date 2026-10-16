/*---
description: 龍圖可買賣丹藥的藥鋪房間（HERB）。任何房間以 inherit HERB
             取代 inherit ROOM，即可提供方士的 herblist、buy、sell 交易入口。
note: 不屬於丹藥交易的指令一律 return 0，交回原版處理（例如向 NPC 購買）。
      丹藥名稱、價格、技能需求一律由 /daemon/misc/herb_catalog 讀取丹藥檔，
      不在本檔另列名單。
      套用的房間不可呼叫 replace_program(ROOM)，否則本檔功能會被丟棄。
---*/

#define HERB_D "/daemon/misc/herb_d"

inherit ROOM;

void init()
{
    ::init();
    add_action("do_herblist", "herblist");
    add_action("do_herb_buy", "buy");
    add_action("do_herb_sell", "sell");
}

int do_herblist(string arg) { return HERB_D->cmd_herblist(this_player(), arg); }
int do_herb_buy(string arg) { return HERB_D->cmd_buy(this_player(), arg); }
int do_herb_sell(string arg) { return HERB_D->cmd_sell(this_player(), arg); }
