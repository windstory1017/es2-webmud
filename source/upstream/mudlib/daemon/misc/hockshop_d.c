/*---
description: 當鋪連線的共用倉庫（五堂鎮、李家村、喬陰縣三間當鋪共用）。
design: docs/PAWNSHOP_NETWORK.md（2026-10-23）
note: - 賣斷的物品上架；完全相同的物品疊成一項，清單依每一項第一次進貨的先後排序。
      - 上架最多 MAX_GROUPS 項，滿了從最舊的一項整項丟掉。
      - 同一項最多上架 MAX_PER_GOODS 件，超過的不上架（賣出的玩家照常拿錢）。
      - 典當的物品（hockshop_pawned）也放在這裡，不上架、不算項數、不會被擠掉，
        任何一間當鋪都能憑當票贖回。
      - 重開伺服器不保留（倉庫只在記憶體中）。
---*/

#define STORAGE_FILE "/std/room/hockshop_storage"
#define MAX_GROUPS   200
#define MAX_PER_GOODS 10     // 同一項最多上架幾件，超過的不上架（使用者定案，2026-10-25）

static object storage;
static int seq;

void create()
{
    seteuid(getuid());
}

// 常駐，不讓系統清掉（清掉的話倉庫裡的貨會跟著消失）。
int clean_up() { return 1; }

object query_storage()
{
    if( !objectp(storage) ) {
        storage = new(STORAGE_FILE);
        storage->set("shop_ob", this_object());
    }
    return storage;
}

int is_pawned(object ob) { return ob->query("hockshop_pawned") ? 1 : 0; }

// 完全相同的物品才疊在一起：同一個物品檔、名稱、價值、能力（附加值）、詞綴都一樣。
// 本來就是一疊的物品（饅頭等）每一疊各自一項；上架時由 add_goods() 併進同種的那一疊。
string goods_key(object ob)
{
    if( function_exists("query_amount", ob) ) return "combined:" + file_name(ob);
    return sprintf("%s|%s|%d|%O|%O|%O", base_name(ob), "" + ob->query("name"),
        ob->query("value"), ob->query("affix"),
        ob->query("apply_weapon"), ob->query("apply_armor"));
}

// 物品上架時記下疊加的依據（hockshop_key），之後排清單不必每件重算。
string query_key(object ob)
{
    string key = ob->query("hockshop_key");

    if( !stringp(key) ) ob->set("hockshop_key", key = goods_key(ob));
    return key;
}

// 上架的貨物，依項分組並依第一次進貨的先後排序：({ ({ 物品, ... }), ... })
mixed *query_groups()
{
    mapping groups = ([]), first = ([]);
    string *keys, key;
    int s;

    foreach(object ob in all_inventory(query_storage())) {
        if( is_pawned(ob) ) continue;
        key = query_key(ob);
        s = ob->query("hockshop_seq");
        if( undefinedp(groups[key]) ) { groups[key] = ({ ob }); first[key] = s; }
        else {
            groups[key] += ({ ob });
            if( s < first[key] ) first[key] = s;
        }
    }
    keys = sort_array(keys(groups), (: $(first)[$1] - $(first)[$2] :));
    return map_array(keys, (: $(groups)[$1] :));
}

// 典當物：({ 物品, ... })
object *query_pawned()
{
    return filter_array(all_inventory(query_storage()), (: is_pawned :));
}

// 賣斷的物品上架。新的一項要上架而已滿 MAX_GROUPS 項時，從最舊的一項整項丟掉。
int add_goods(object ob)
{
    mixed *groups;
    mapping seen = ([]);
    string key, k;
    int found, same;

    if( !objectp(ob) ) return 0;
    ob->delete("hockshop_pawned");

    // 本來就是一疊的物品：放進倉庫不會自動合併，所以併進已上架的同種那一疊（整疊一起賣）。
    if( function_exists("query_amount", ob) )
        foreach(object old in all_inventory(query_storage()))
            if( old != ob && !is_pawned(old) && base_name(old) == base_name(ob) ) {
                old->add_amount(ob->query_amount());
                destruct(ob);
                return 1;
            }

    ob->delete("hockshop_key");
    key = query_key(ob);
    // 先只數有幾項、這一項是否已上架；需要丟掉舊貨時才排清單（一次賣很多件時省運算）。
    foreach(object o in all_inventory(query_storage())) {
        if( is_pawned(o) ) continue;
        k = query_key(o);
        if( k == key ) { found = 1; same++; }
        seen[k] = 1;
    }
    // 這一項已有 MAX_PER_GOODS 件：這一件不上架（賣出的玩家照常拿錢，由當鋪房間處理）。
    if( same >= MAX_PER_GOODS ) {
        destruct(ob);
        return 0;
    }
    if( !found && sizeof(seen) >= MAX_GROUPS ) {
        groups = query_groups();
        while( sizeof(groups) >= MAX_GROUPS ) {
            foreach(object old in groups[0])
                if( objectp(old) ) destruct(old);
            groups = groups[1..];
        }
    }
    ob->set("hockshop_seq", ++seq);
    return ob->move(query_storage());
}

// 典當物收進倉庫（不上架）。
int add_pawned(object ob)
{
    if( !objectp(ob) ) return 0;
    ob->set("hockshop_pawned", 1);
    return ob->move(query_storage());
}
