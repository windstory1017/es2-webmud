

inherit ROOM;

// 當鋪連線（docs/PAWNSHOP_NETWORK.md，2026-10-23）：三間當鋪共用一個倉庫，
// 由 HOCKSHOP_D 管理；賣斷的物品上架並疊加，典當物任何一間當鋪都能贖回。
#define HOCKSHOP_D "/daemon/misc/hockshop_d"

static object storage;

void setup()
{
    ::setup();
    seteuid(getuid());
    storage = HOCKSHOP_D->query_storage();
    set("storage_ob", storage);
}

void init()
{
    add_action("do_value", "value");
    add_action("do_pawn", "pawn");
    add_action("do_sell", "sell");
    add_action("do_retrieve", "retrieve");
    add_action("do_list", "list");
    add_action("do_buy", "buy");
    add_action("do_store", "store");
}

string value_string(int value)
{
    if( value < 1 ) return "不值一文";
    if( value < 100 )
        return chinese_number(value) + "文錢";
    return chinese_number(value/100) + "兩"
        + (value%100? "又" + chinese_number(value%100) + "文錢": "");
}

void pay_player(object who, int amount)
{
    object ob;

    if( amount < 1 ) amount = 1;
    if( amount/100 ) {
        ob = new(SILVER_OB);
        ob->set_amount(amount/100);
        if( !ob->move(who) ) ob->move(environment(who));
        amount %= 100;
    }
    if( amount ) {
        ob = new(COIN_OB);
        ob->set_amount(amount);
        if( !ob->move(who) ) ob->move(environment(who));
    }
}

int do_value(string arg)
{
    object ob;
    int value;

    if( !arg || !(ob = present(arg, this_player())) )
        return notify_fail("你要拿什麼物品給當鋪估價﹖\n");

    if( ob->query("money_id") )
        return notify_fail("這是「錢」﹐你沒見過嗎﹖\n");

    value = ob->query("value");
    if( value < 1 ) {
        printf("%s一文不值。\n", ob->query("name"));
        return 1;
    }
    // 打倒 NPC 撿來的物品（sinister）與一般物品同價（使用者指定，2026-10-23；原本只給 1%）。
    printf("%s價值%s。\n如果你要典當(pawn)﹐可以拿到%s及一張當票。\n如果賣斷(sell)﹐可以拿到%s。\n",
            ob->query("name"), value_string(value),
            value_string(value * 60 / 100), value_string(value * 80 / 100));

    return 1;
}

int do_pawn(string arg)
{
    object ob, stamp;
    int value;

    if( !arg || !(ob = present(arg, this_player())) )
        return notify_fail("你要典當什麼物品﹖\n");

    if( ob->query("money_id") )    return notify_fail("你要當「錢」﹖\n");

    value = ob->query("value");
    if( value < 1 ) return notify_fail("這樣東西不值錢。\n");

    if (ob->query("no_sell")) return notify_fail("這樣東西不能典當。\n");

    message_vision("$N把身上的" + ob->query("name") + "拿出來典當了"
        + value_string(value * 60 / 100) + "﹐換得一張當票。\n", this_player());

    pay_player(this_player(), value * 60 / 100);

    HOCKSHOP_D->add_pawned(ob);
    stamp = new(__DIR__"obj/pawn_stamp");
    stamp->set("pawned_item", ob);
    stamp->set("long", "茲收到\n    " + this_player()->name(1)
        + "典當" + ob->name() + "乙" + ob->query("unit")
        + "訖﹐破爛缺損不堪。\n    贖回 (retrieve) 應付" + value_string(value * 120/100)
        + "整﹐一手交錢﹐一手交貨。\n");
    stamp->move(this_player());

    return 1;
}

int sell_item(object ob)
{
    int value;

    if( ob->query("money_id") )    return notify_fail("你要賣「錢」﹖\n");
    value = ob->query("value");
    if( value < 1 ) return notify_fail("這樣東西不值錢。\n");

    if (ob->query("no_sell")) return notify_fail("這樣東西不能賣。\n");

    message_vision("$N把身上的" + ob->query("name") + "賣掉。\n",
        this_player());

    // 打倒 NPC 撿來的物品（sinister）與一般物品同價：賣斷 80%（使用者指定，2026-10-23）。
    pay_player(this_player(), value * 80 / 100);

    ob->delete("sinister");
    // 上架到共用倉庫（相同物品疊成一項；滿 200 項時從最舊的一項整項丟掉；
    // 同一項已有 10 件的話，這一件不上架，錢照付）。
    HOCKSHOP_D->add_goods(ob);

    return 1;
}

int do_sell(string arg)
{
    object ob;

    if( !arg ) return notify_fail("你要賣斷什麼物品﹖\n");

    if( arg=="all" ) {
        object *inv;
        
        inv = all_inventory(this_player());
        foreach(ob in inv) {
            if(!ob) continue;
            if( ob->query("keep_this") ) continue;
            sell_item(ob);
        }
        write("Ok。\n");
        return 1;
    }

    if( !(ob = present(arg, this_player())) )
        return notify_fail("你要賣斷什麼物品﹖\n");

    return sell_item(ob);
}

int do_retrieve(string arg)
{
    object stamp, ob;
    int value;

    if( !arg || arg=="" || !(stamp = present(arg, this_player())) )
        return notify_fail("你要贖回哪一張當票上的典物﹖\n");

    if( !stamp->id("pawn stamp") )
        return notify_fail("這不是當票。\n");

    if( !(ob = stamp->query("pawned_item")) ) {
        write("這張當票上的典物因為你太久沒有來贖﹐已經被當鋪賣掉了。\n");
        write("你隨手將當票撕掉扔了。\n");
        destruct(stamp);
        return 1;
    }

    // 三間當鋪共用倉庫：任何一間都能贖回。
    if( !present(ob, HOCKSHOP_D->query_storage()) || !ob->query("hockshop_pawned") )
        return notify_fail("這張當票上的典物已經不在當鋪裡了。\n");

    value = (int)ob->query("value") * 120 / 100;
    if( (int)this_player()->can_afford(value) != 1 )
        return notify_fail("你身上的(零)錢不夠。\n");

    message_vision("$N取出當票﹐花了" + value_string(value) + "贖回了"
        + ob->name() + "。\n", this_player());

    this_player()->pay_money(value);
    ob->delete("hockshop_pawned");
    ob->delete("hockshop_seq");
    ob->delete("hockshop_key");
    if( !ob->move(this_player()) ) ob->move(environment(this_player()));
    destruct(stamp);

    return 1;
}

// 一項貨物的單價：疊加的物品是單件價格；本來就是一疊的（饅頭等）是整疊價格。
int goods_price(object ob) { return (int)ob->query("value") * 2; }

// 一次 write() 最多 8192 個位元組（中文字一個佔 3），清單分段送出，每段留在這個長度以內。
#define LIST_CHUNK 6000

int do_list(string arg)
{
    mixed *groups;
    object ob;
    string line, chunk, count;
    int i, n, shown;

    groups = HOCKSHOP_D->query_groups();
    if( !sizeof(groups) && !wizardp(this_player()) )
        return notify_fail("當鋪裡現在沒有可供出售的貨物。\n");

    chunk = "當鋪裡現在有以下貨物出售﹕\n\n";
    // 名稱、數量與價格依畫面寬度補空白；價格靠右對齊。
    // list <名稱> 只列出符合的項目，但編號仍是完整清單上的編號，與 buy <編號> 一致。
    for(i = 0; i < sizeof(groups); i++) {
        ob = groups[i][0];
        if( arg && !ob->id(arg) ) continue;
        n = sizeof(groups[i]);
        count = n > 1 ? "共 " + n + " " + (ob->query("unit") ? ob->query("unit") : "件") : "";
        line = sprintf("     %-5d  %s %s %s\n", i + 1,
            cjk_pad(ob->short(), 40), cjk_pad(count, 10, 1),
            cjk_pad(value_string(goods_price(ob)), 22, 1));
        if( strlen(chunk) + strlen(line) > LIST_CHUNK ) {
            write(chunk);
            chunk = "";
        }
        chunk += line;
        shown++;
    }
    if( arg && !shown && !wizardp(this_player()) )
        return notify_fail("當鋪裡現在沒有可供出售的貨物。\n");
    if( wizardp(this_player()) ) {
        object *pawned = HOCKSHOP_D->query_pawned();
        mapping kinds = ([]);
        string *order = ({}), key;

        // 典當中的物品，相同的合成一行並標數量。
        foreach(ob in pawned) {
            key = ob->short();
            if( undefinedp(kinds[key]) ) { kinds[key] = 0; order += ({ key }); }
            kinds[key]++;
        }
        if( sizeof(order) ) {
            if( strlen(chunk) > LIST_CHUNK - 200 ) { write(chunk); chunk = ""; }
            chunk += "\n（巫師）典當中的物品﹕\n";
            foreach(key in order) {
                line = "            " + key + (kinds[key] > 1 ? "  共 " + kinds[key] + " 件" : "") + "\n";
                if( strlen(chunk) + strlen(line) > LIST_CHUNK ) { write(chunk); chunk = ""; }
                chunk += line;
            }
        }
    }
    if( strlen(chunk) ) write(chunk);
    return 1;
}

// buy <名稱>、buy <名稱> <第幾項>、buy <清單編號>；一次買一件，不支援一次買多件。
int do_buy(string arg)
{
    mixed *groups, *match;
    object ob;
    string name_part;
    int index, price;

    if( !arg ) return notify_fail("你要買什麼東西﹖\n");

    groups = HOCKSHOP_D->query_groups();
    if( !sizeof(groups) ) return notify_fail("當鋪裡現在沒有貨物。\n");

    if( sscanf(arg, "%d", index) == 1 && ("" + index) == arg ) {
        // 照清單編號
        if( index < 1 || index > sizeof(groups) )
            return notify_fail("當鋪的清單上沒有這一項。\n");
        match = groups[index - 1];
    } else {
        // 照名稱；「buy dagger 2」= 清單上第二項 dagger。sscanf 失敗時不能動到 arg。
        if( sscanf(arg, "%s %d", name_part, index) == 2 ) arg = name_part;
        else index = 1;
        groups = filter_array(groups, (: $1[0]->id($(arg)) :));
        if( index < 1 || index > sizeof(groups) )
            return notify_fail("你要買什麼東西﹖\n");
        match = groups[index - 1];
    }
    ob = match[0];
    price = goods_price(ob);

    if( (int)this_player()->can_afford(price) != 1 )
        return notify_fail("你的(零)錢不夠。\n");
    message_vision("$N掏出錢買下" + (function_exists("query_amount", ob)
        ? ob->short() : "一" + ob->query("unit") + ob->name()) + "。\n", this_player());
    this_player()->pay_money(price);
    ob->delete("hockshop_seq");
    ob->delete("hockshop_key");
    // 付了錢卻拿不動時放在地上。
    if( !ob->move(this_player()) )
        ob->move(environment(this_player()));
    return 1;
}

int do_store(string arg)
{
    if( !wizardp(this_player()) ) return 0;
    this_player()->move(HOCKSHOP_D->query_storage());
    write("Ok.\n");
    return 1;
}

