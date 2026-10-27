

#include <dbase.h>
#include <name.h>

static mapping stock;

private void
reset()
{
    if( !environment() ) return;
    stock = copy(query("merchandise"));
}

// 商品順序（使用者定案，2026-10-23）：list 顯示的順序和 buy <名稱> <第幾個> 的計數
// 用同一份排序（價錢由低到高，同價依檔名），兩邊才會一致。
private int sort_goods(string a, string b)
{
    int va = a->query("value"), vb = b->query("value");
    if( va != vb ) return va - vb;
    return strcmp(a, b);
}

string *query_goods_order()
{
    mapping list = query("merchandise");
    if( !mapp(list) ) return ({});
    return sort_array(keys(list), (: sort_goods :));
}

int query_stock(string item)
{
    if( !mapp(stock) ) stock = copy(query("merchandise"));
    return stock[item];
}

// affirm_merchandise()
//
// This is an apply function interfacing with the standard 'buy' command
// which is called when an player attemp to 'buy xxx from yyy' while xxx
// is passed via the parameter what and yyy is this_object() (must be
// present to the player). If we affirmed that we have such item for sell,
// buy_ob should return an string, or int, or anything that identifies
// this item we have in later bargain. Else, 0 is returned if we are not
// selling such item.

varargs mixed
affirm_merchandise(object buyer, string what, int amount)
{
    mapping list;
    string item, name_part;
    int i, index, count;

    if( amount < 1 ) amount = 1;
    // 「buy skin 2 from xxx」表示第二種叫 skin 的商品。sscanf 失敗時不能動到 what，
    // 否則像 "water skin" 這種有空格的名稱會被截成 "water"。
    if( sscanf(what, "%s %d", name_part, index) == 2 ) what = name_part;
    else index = 1;

    if( !mapp(list = query("merchandise")) )
	return notify_fail(name() + "並沒有賣 " + what + " 這種東西。\n");

    if( !mapp(stock) ) stock = copy(list);

    // 和 list 一樣的順序，只數還有存貨（list 上看得到）的商品。
    foreach(item in query_goods_order())
    {
	if( !item->id(what) ) continue;
	if( stock[item] < 1 ) {
	    if( !count ) count = -1;   // 記下有同名但賣完的商品
	    continue;
	}
	count = 1;
	if( --index ) continue;
	if( stock[item] < amount )
	    return notify_fail(item->name() + "只剩下" + chinese_number(stock[item])
		+ (item->query("base_unit") || item->query("unit") || "個") + "了。\n");
	return item;
    }
    if( count < 0 )
	return notify_fail("這樣東西已經賣完了﹐待會兒再來吧。\n");
    return notify_fail(name() + "並沒有賣 " + what + " 這種東西。\n");
}

int
query_trading_price(string handle)
{
    mapping list;

    if( !mapp(list = query("merchandise")) )
	return 0;

    if( undefinedp(list[handle]) ) return 0;

    return handle->query("value");
}

// 商店賣的武器一律是普通品，不帶隨機附加屬性（使用者決定，2026-10-16）。
// 武器產生時已經擲過詞綴（adm/daemons/enhanced.c 的 roll_affix），這裡改回普通品。
private object plain_goods(object ob)
{
    if( objectp(ob) && !undefinedp(ob->query("affix")) )
        ENHANCE_D->apply_affix(ob, ([]));
    return ob;
}

// amount：一次買幾個。可堆疊的商品（COMBINED_ITEM）合成一堆交貨；其他商品一件一件交，
// 身上拿不動（太重或放不下）的部分直接銷毀，不掉在地上，庫存與價錢照算。
varargs void
deliver_merchandise(object me, string what, int amount)
{
    mapping list;
    string unit, item_name;
    object ob;
    int got, i;

    if ( !living(this_object()) ) return;

    list = query("merchandise");
    if( !mapp(list) || undefinedp(list[what]) ) return;
    if( amount < 1 ) amount = 1;

    ob = plain_goods(new(what));
    item_name = ob->query("name");

    if( ob->query_volume() ) {
	// Special process for liquid merchandise.
	object container_ob;
	string container;
	if( !(container = ob->query("default_container")) )
	    container = "/obj/bottle";
	for( i = 0; i < amount; i++ ) {
	    if( i ) ob = plain_goods(new(what));
	    container_ob = new(container);
	    unit = container_ob->query("container_unit");
	    ob->move(container_ob);
	    if( container_ob->move(me) ) got++;
	    else destruct(container_ob);
	}
    } else if( function_exists("set_amount", ob) ) {
	// 可堆疊：一次給一堆，拿不動就逐次減少數量，直到拿得動為止。
	unit = ob->query("base_unit") || ob->query("unit");
	got = amount;
	ob->set_amount(got);
	while( got > 0 && !ob->move(me) ) {
	    got--;
	    if( got > 0 ) ob->set_amount(got);
	}
	if( got == 0 ) destruct(ob);
    } else {
	unit = ob->query("unit");
	for( i = 0; i < amount; i++ ) {
	    if( i ) ob = plain_goods(new(what));
	    if( ob->move(me) ) got++;
	    else destruct(ob);
	}
    }

    if( !unit ) unit = "個";
    // 用 tell_object：店小二這類延遲交貨的商人，交貨時 this_player() 不一定是買家。
    tell_object(me, "你向" + name() + "買下" + chinese_number(amount) + unit + item_name + "。\n");
    if( got < amount )
	tell_object(me, "你身上拿不動，其中" + chinese_number(amount - got) + unit + item_name + "就這麼不見了。\n");

    // Reduce stock account;
    if( !mapp(stock) ) stock = copy(query("merchandise"));
    stock[what] -= amount;
}

private string
price_string(int v)
{
    if( v%10000 == 0 ) return chinese_number(v/10000) + "兩黃金";
    if( v%100 == 0 ) return chinese_number(v/100) + "兩銀子";
    return chinese_number(v) + "文錢";
}

// 計算字串的全形字數與半形字數（略過 ANSI 顏色碼），對齊用。
private int *glyph_count(string str)
{
    int w = 0, n = 0, in_ansi = 0;

    foreach(string ch in explode(str, "")) {
	if( in_ansi ) { if( ch == "m" ) in_ansi = 0; continue; }
	if( ch == "\x1b" ) { in_ansi = 1; continue; }
	if( strlen(ch) > 1 && ch >= "⺀" ) w++;
	else n++;
    }
    return ({ w, n });
}

// 補到 wide 個全形字、narrow 個半形字：不論網頁字型的中文字寬為何，每一行都會對齊。
private string pad_glyphs(string str, int wide, int narrow, int right_align)
{
    int *c = glyph_count(str);
    string pad = repeat_string("　", wide > c[0] ? wide - c[0] : 0)
	+ repeat_string(" ", narrow > c[1] ? narrow - c[1] : 0);
    return right_align ? pad + str : str + pad;
}

int
do_vendor_list(string arg)
{
    mapping goods;
    string list, *names, *prices, *items;
    int i, wide, narrow, pwide, pnarrow, *c;

    if( !mapp(goods = query("merchandise")) ) return 0;
    if( arg && !id(arg) ) return 0;

    // 只列還有存貨的商品，順序和 buy 的計數相同。
    items = filter(query_goods_order(), (: query_stock($1) > 0 :));
    if( !sizeof(items) ) {
	write( name() + "的貨物已經全部賣光了，下次早一點來吧！\n");
	return 1;
    }

    // 名稱與價錢欄都以最寬的一項為準對齊（使用者定案，2026-10-23）；
    // 名稱與「：」之間多空四個半形（2026-10-24）。
    names = map(items, (: $1->short(1) :));
    prices = map(items, (: price_string($1->query("value")) :));
    for( i = 0; i < sizeof(items); i++ ) {
	c = glyph_count(names[i]);
	if( c[0] > wide ) wide = c[0];
	if( c[1] > narrow ) narrow = c[1];
	c = glyph_count(prices[i]);
	if( c[0] > pwide ) pwide = c[0];
	if( c[1] > pnarrow ) pnarrow = c[1];
    }

    list = "";
    for( i = 0; i < sizeof(items); i++ )
	list += "  " + pad_glyphs(names[i], wide, narrow, 0) + "    ："
	    + pad_glyphs(prices[i], pwide, pnarrow, 1) + "\n";

    write("你可以購買下列這些東西：\n-------------------------\n" + list);
    return 1;
}
