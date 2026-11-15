

#include <ansi.h>
#include <combat.h>

string enhance_weapon(object ob, int bonus, string weapon_type)
{
    int major = random(200), minor = random(1000);
    string new_name = "";
    string id = ob->query("id");
    mapping apply = ([]);

    if( major < 3 )
        switch( random(14) ) {
        case 0:
            new_name += CYN "青銅"; apply["defense"] = 10;
            id = "bronze " + id;
            break;
        case 1:
            new_name += RED "鑌鐵"; apply["damage"] = 3;
            id = "iron " + id;
            break;
        case 2:
            new_name += HIW "點鋼"; apply["attack"] = 5; apply["damage"] = 2;
            id = "steel " + id;
            break;
        case 3:
            new_name += HIB "青鋼"; apply["attack"] = 5; apply["defense"] = 5;
            id = "steel " + id;
            break;
        case 4:
            new_name += HIK "黑鐵"; apply["damage"] = 2; apply["defense"] = 5;
            id = "iron " + id;
            break;
        case 5:
            new_name += MAG "松紋"; apply["str"] = 1; apply["attack"] = 5;
            id = "fine " + id;
            break;
        case 6:
            new_name += HIM "雕花"; apply["dex"] = 1; apply["defense"] = 5;
            id = "fine " + id;
            break;
        case 7:
            new_name += HIK "纏布"; apply["cps"] = 1; apply["armor"] = 3;
            id = "wrapped-hilt " + id;
            break;
        case 8:
            new_name += HIK "鐵棘"; apply["str"] = 1; apply["damage"] = 2;
            id = "thorny " + id;
            break;
        case 9:
            new_name += HIW  "太極"; apply["wis"] = 1; apply["defense"] = 5;
            id = "taiji " + id;
            break;
        case 10:
            new_name += HIW  "陰陽"; apply["int"] = 1; apply["attack"] = 5;
            id = "yinyang " + id;
            break;
        case 11:
            new_name += HIR "硃砂"; apply["magic"] = 5; apply["spells"] = 5;
            id = "cinnabar " + id;
            break;
        case 12:
            new_name += HIY "日月"; apply["con"] = 1; apply["magic"] = 5;
            id = "glowing " + id;
            break;
        case 13:
            new_name += HIK "厭火"; apply["con"] = 1; apply["damage"] = 2;
            id = "yenholdish " + id;
            break;
    }
    else if( major < 6 )
        switch( random(22) ) {
        case 0:
            new_name += HIW "精鋼"; apply["attack"] = 10; apply["damage"] = 3;
            id = "highsteel " + id;
            break;
        case 1:
            new_name += HIW "白銀"; apply["defense"] = 15;
            id = "silver " + id;
            break;
        case 2:
            new_name += HIK "黑鋼"; apply["attack"] = 15;
            id = "darksteel " + id;
            break;
        case 3:
            new_name += YEL "古錠"; apply["defense"] = 10; apply["cps"] = 1;
            id = "darkmetal " + id;
            break;
        case 4:
            new_name += RED "雛鐵"; apply["attack"] = 10; apply["defense"] = 10;
            id = "iron" + id;
            break;
        case 5:
            new_name += HIR "彤雲"; apply["wis"] = 1; apply["cor"] = 1;
            id = "cloudy " + id;
            break;
        case 6:
            new_name += MAG "紫電"; apply["dex"] = 1; apply["str"] = 1;
            id = "thunder " + id;
            break;
        case 7:
            new_name += HIY "金鑲"; apply["con"] = 1; apply["int"] = 1;
            id = "golden " + id;
            break;
        case 8:
            new_name += HIW "太玄"; apply["spi"] = 1; apply["spells"] = 10;
            id = "mystic " + id;
            break;
        case 9:
            new_name += HIB "太清"; apply["wis"] = 1; apply["spells"] = 10;
            id = "mystic " + id;
            break;
        case 10:
            new_name += HIC "太乙"; apply["int"] = 1; apply["spells"] = 10;
            id = "mystic " + id;
            break;
        case 11:
            new_name += HIK "太陰"; apply["int"] = 2;
            id = "moon " + id;
            break;
        case 12:
            new_name += HIY "太陽"; apply["con"] = 2;
            id = "sun " + id;
            break;
        case 13:
            new_name += HIB "辟妖"; apply["magic"] = 15;
            id = "demonbane " + id;
            break;
        case 14:
            new_name += BLU "辟邪"; apply["spells"] = 15;
            id = "devilbane " + id;
            break;
        case 15:
            new_name += HIK "鎮鬼"; apply["magic"] = 10; apply["spells"] = 10;
            id = "nether " + id;
            break;
        case 16:
            new_name += RED "赤血"; apply["damage"] = 5;
            id = "blood " + id;
            break;
        case 17:
            new_name += HIR "碧血"; apply["armor"] = 5;
            id = "blood " + id;
            break;
        case 18:
            new_name += CYN "無腸"; apply["int"] = 3; apply["con"] = -1;
            id = "woochanian " + id;
            break;
        case 19:
            new_name += HIG "焦僥"; apply["dex"] = 3; apply["str"] = -1;
            id = "jiaojao " + id;
            break;
        case 20:
            new_name += HIM "紫陽"; apply["cps"] = 2;
            id = "dawn " + id;
            break;
        case 21:
            new_name += HIC "青陽"; apply["wis"] = 2;
            id = "shining " + id;
            break;
    }

    if( new_name=="" ) return ob->name();
    
    if( minor < 3 )
        switch( random(13) ) {
        case 4:
            new_name += HIR "火炎";
            id += " of flame";
            break;
        case 5:
            new_name += HIB "寒冰";
            id += " of freeze";
            break;
        case 6:
            new_name += HIC "風波";
            id += " of wind";
            break;
        case 7:
            new_name += HIB "震雷";
            id += " of lightning";
            break;
        case 8:
            new_name += YEL "羅漢";
            id += " of guardian";
            break;
        case 9:
            new_name += HIW "天龍";
            id += " of wyvern";
            break;
        case 10:
            new_name += HIR "赤龍";
            id += " of red dragon";
            break;
        case 11:
            new_name += HIY "金鷹";
            id += " of eagle";
            break;
        case 12:
            new_name += HIC "天鷹";
            id += " of falcon";
            break;
    }

    if( new_name != "" ) {
        mixed wield_as;
        string wield_skill;
        new_name += weapon_type + NOR;
        ob->set("name", new_name);
        ob->set("id", id);
        // Apply enchantment to the weapon.
        wield_as = ob->query("wield_as");
        if( arrayp(wield_as) ) 
            foreach(wield_skill in wield_as)
                ob->set("apply_weapon/" + wield_skill, apply);
        else
            ob->set("apply_weapon/" + wield_as, apply);
        return new_name;
    }

    return ob->name();
}


/* =====================================================================
 * 武器附加屬性（使用者提供的「武器附加屬性表」，obj/area/obj 的 12 種一般武器使用）
 *
 * 武器產生時擲一次：普通 80%、只有前綴 15%、前綴加後綴 5%（必須有前綴才會有後綴）。
 * 前綴來自表一、表二（同名的效果合併），後綴來自表三。名稱為「前綴＋後綴＋原名」，
 * 例如「黑鋼火炎長劍」；每個修飾讓價值變成 1.5 倍。
 * 結果記在 query("affix")（([ "prefix": 名稱, "suffix": 名稱 ])），家園儲物箱會照存。
 * =====================================================================
 */
private mapping AFFIX_PREFIX = ([
    "形天": ([ "str": 2, "con": 1 ]),
    "雕花": ([ "str": 1, "cor": 1 ]),
    "彤雲": ([ "str": 1, "spells": 15 ]),
    "修羅": ([ "cor": 2, "intimidate": 15 ]),
    "赤血": ([ "cor": 1, "attack": 15 ]),
    "無腸": ([ "int": 3 ]),
    "太陰": ([ "int": 2 ]),
    "金鑲": ([ "int": 1, "attack": 15 ]),
    "陰陽": ([ "int": 1 ]),
    "巫首": ([ "spi": 2 ]),
    "辟妖": ([ "spi": 1, "magic": 15 ]),
    "太玄": ([ "spi": 1, "spells": 15 ]),
    "紫陽": ([ "cps": 3 ]),
    "古錠": ([ "cps": 2, "defense": 15 ]),
    "碧血": ([ "cps": 1, "armor": 15 ]),
    "焦僥": ([ "dex": 3 ]),
    "太陽": ([ "con": 2 ]),
    "厭火": ([ "con": 1 ]),
    "青陽": ([ "wis": 2 ]),
    "太極": ([ "wis": 1 ]),
    "太乙": ([ "wis": 1, "spells": 15 ]),
    "辟邪": ([ "wis": 1, "magic": 15 ]),
    "黑鋼": ([ "damage": 15, "attack": 10 ]),
    "精鋼": ([ "damage": 10, "attack": 15 ]),
    "黑鐵": ([ "damage": 5 ]),
    "纏布": ([ "armor": 10 ]),
    "青鋼": ([ "armor": 5, "attack": 5 ]),
    "雛鐵": ([ "attack": 10, "force": 10 ]),
    "點鋼": ([ "attack": 10 ]),
    "鐵棘": ([ "attack": 5, "defense": 5 ]),
    "青銅": ([ "defense": 10 ]),
    "鎮鬼": ([ "magic": 15, "spells": 15 ]),
    "日月": ([ "magic": 10 ]),
    "硃砂": ([ "spells": 10 ]),
    "白銀": ([ "parry": 10 ]),
    "紫電": ([ "dodge": 10 ]),
]);
private mapping AFFIX_SUFFIX = ([
    "火炎": ([ "str": 3 ]),
    "震雷": ([ "cor": 3 ]),
    "風波": ([ "dex": 3 ]),
    "羅漢": ([ "spi": 2 ]),
    "寒冰": ([ "cps": 3 ]),
    "天龍": ([ "force": 20 ]),
    "赤龍": ([ "intimidate": 30 ]),
    "金鷹": ([ "wittiness": 30 ]),
    "天鷹": ([ "awarness": 50 ]),
]);

// 名稱的顏色：只有前綴時整把用前綴的顏色；有後綴時前綴用前綴的顏色，後綴與武器名用後綴的顏色。
// AFFIX_COLOR 設為 0 則不上色。
#define AFFIX_COLOR 1
private mapping AFFIX_COLOR_CODE = ([
    "形天": HIY,
    "雕花": HIM,
    "彤雲": HIR,
    "修羅": HIR,
    "赤血": RED,
    "無腸": CYN,
    "太陰": HIK,
    "金鑲": HIY,
    "陰陽": HIW,
    "巫首": HIK,
    "辟妖": HIB,
    "太玄": HIW,
    "紫陽": HIM,
    "古錠": YEL,
    "碧血": HIR,
    "焦僥": HIG,
    "太陽": HIY,
    "厭火": HIK,
    "青陽": HIC,
    "太極": HIW,
    "太乙": HIC,
    "辟邪": BLU,
    "黑鋼": HIK,
    "精鋼": HIW,
    "黑鐵": HIK,
    "纏布": HIK,
    "青鋼": HIB,
    "雛鐵": RED,
    "點鋼": HIW,
    "鐵棘": HIK,
    "青銅": CYN,
    "鎮鬼": HIK,
    "日月": HIY,
    "硃砂": HIR,
    "白銀": HIW,
    "紫電": MAG,
    "火炎": HIR,
    "震雷": HIB,
    "風波": HIC,
    "羅漢": HIY,
    "寒冰": HIB,
    "天龍": HIW,
    "赤龍": HIR,
    "金鷹": HIY,
    "天鷹": HIC,
]);

// 有詞綴時原名改顯示成武器種類（長劍、短劍都顯示成「劍」，單刀、鬼頭刀都顯示成「刀」），
// 英文名稱不變，仍可分辨是哪一種。武器可用 set("affix_short_name", ...) 自己指定。
private mapping AFFIX_BASE_SHORT = ([
    "sword": "劍",
    "blade": "刀",
    "dagger": "匕",
    "staff": "杖",
    "axe": "斧",
    "pike": "槍",
    "whip": "鞭",
]);

private string affix_short_name(object ob, string name)
{
    mixed wield_as = ob->query("wield_as");
    string k;

    if( stringp(ob->query("affix_short_name")) ) return ob->query("affix_short_name");
    if( stringp(wield_as) ) wield_as = ({ wield_as });
    if( arrayp(wield_as) )
        foreach(k in wield_as) {
            if( !stringp(k) ) continue;
            k = replace_string(replace_string(k, "twohanded ", ""), "secondhand ", "");
            if( !undefinedp(AFFIX_BASE_SHORT[k]) ) return AFFIX_BASE_SHORT[k];
        }
    return name;
}

private string affix_color(string word)
{
    if( !AFFIX_COLOR || !stringp(word) || undefinedp(AFFIX_COLOR_CODE[word]) ) return 0;
    return AFFIX_COLOR_CODE[word];
}

// 依 affix 重新設定名稱、能力與價值；affix 為空（或 0）就是普通武器。
void apply_affix(object ob, mapping affix)
{
    string base_name, name, prefix, suffix, k;
    int base_value, value, v;
    mapping apply = ([]), part;
    mixed wield_as;

    if( !objectp(ob) ) return;
    if( !mapp(affix) ) affix = ([]);
    if( !stringp(base_name = ob->query("affix_base_name")) ) {
        base_name = ob->query("name");
        ob->set("affix_base_name", base_name);
    }
    if( !(base_value = ob->query("affix_base_value")) ) {
        base_value = ob->query("value");
        ob->set("affix_base_value", base_value);
    }

    prefix = affix["prefix"];
    suffix = affix["suffix"];
    if( !stringp(prefix) || undefinedp(AFFIX_PREFIX[prefix]) ) prefix = 0;
    if( !prefix || !stringp(suffix) || undefinedp(AFFIX_SUFFIX[suffix]) ) suffix = 0;

    name = base_name;
    value = base_value;
    foreach(part in ({ prefix ? AFFIX_PREFIX[prefix] : 0, suffix ? AFFIX_SUFFIX[suffix] : 0 })) {
        if( !mapp(part) ) continue;
        foreach(k, v in part) apply[k] += v;
        value = value * 3 / 2;
    }
    if( prefix ) {
        string pcolor = affix_color(prefix), scolor = affix_color(suffix);
        name = affix_short_name(ob, name);
        if( suffix ) {
            name = (pcolor ? pcolor + prefix + NOR : prefix)
                 + (scolor ? scolor + suffix + name + NOR : suffix + name);
        } else {
            name = pcolor ? pcolor + prefix + name + NOR : prefix + name;
        }
    }

    ob->set("name", name);
    ob->set("value", value);
    if( prefix ) ob->set("affix", suffix ? ([ "prefix": prefix, "suffix": suffix ]) : ([ "prefix": prefix ]));
    else ob->set("affix", ([]));

    wield_as = ob->query("wield_as");
    if( stringp(wield_as) ) wield_as = ({ wield_as });
    if( arrayp(wield_as) )
        foreach(k in wield_as) {
            mapping base, merged;
            string key;
            // 武器原本的特性（例如釣竿的捕魚技巧）先記下來，詞綴加在它上面，不會蓋掉。
            if( undefinedp(ob->query("affix_base_apply/" + k)) )
                ob->set("affix_base_apply/" + k,
                    mapp(base = ob->query("apply_weapon/" + k)) ? copy(base) : ([]));
            merged = copy(ob->query("affix_base_apply/" + k));
            foreach(key, v in apply) merged[key] += v;
            if( sizeof(merged) ) ob->set("apply_weapon/" + k, merged);
            else ob->delete("apply_weapon/" + k);
        }
}

// 武器產生時呼叫：擲骰決定前綴、後綴並套用。
// chance_mult 是出詞綴機會的倍數（不給就是 1 倍：前綴 20%、前綴＋後綴 5%；3 倍：前綴 60%、前綴＋後綴 15%）。
varargs void roll_affix(object ob, int chance_mult)
{
    int r = random(100);
    string *pre = keys(AFFIX_PREFIX), *suf = keys(AFFIX_SUFFIX);
    mapping affix = ([]);

    if( chance_mult < 1 ) chance_mult = 1;
    if( r >= 100 - 20 * chance_mult ) affix["prefix"] = pre[random(sizeof(pre))];
    if( r >= 100 - 5 * chance_mult ) affix["suffix"] = suf[random(sizeof(suf))];
    apply_affix(ob, affix);
}

/* =====================================================================
 * 衣服附加屬性（鳳娘白甌繡花，設計見 docs/EMBROIDERY.md）
 *
 * 前綴表沿用武器的 AFFIX_PREFIX（效果、顏色都相同），只是名稱像武器的幾個換成布料、針線的說法：
 * 黑鋼→錦繡、精鋼→金絲、黑鐵→墨染、點鋼→銀針、雛鐵→雲紋、青鋼→青綾、鐵棘→棘繡。
 * 後綴與武器相同。名稱為「前綴＋後綴＋衣」，每個修飾讓價值變成 1.5 倍。
 * 結果記在 query("affix")，與武器的 affix 同格式。
 * =====================================================================
 */
private mapping CLOTH_RENAME = ([
    "黑鋼": "錦繡", "精鋼": "金絲", "黑鐵": "墨染", "點鋼": "銀針",
    "雛鐵": "雲紋", "青鋼": "青綾", "鐵棘": "棘繡",
]);
private mapping cloth_prefix_tab, cloth_color_tab;

private void init_cloth_tables()
{
    string k, n;

    if( mapp(cloth_prefix_tab) ) return;
    cloth_prefix_tab = ([]);
    cloth_color_tab = ([]);
    foreach(k in keys(AFFIX_PREFIX)) {
        n = stringp(CLOTH_RENAME[k]) ? CLOTH_RENAME[k] : k;
        cloth_prefix_tab[n] = AFFIX_PREFIX[k];
        if( !undefinedp(AFFIX_COLOR_CODE[k]) ) cloth_color_tab[n] = AFFIX_COLOR_CODE[k];
    }
}

// 依 affix 重新設定衣服的名稱、能力與價值；affix 為空就是沒有詞綴。
// 衣服原本的能力記在 cloth_base_apply，原名記在 cloth_base_name，原價記在 cloth_base_value。
void apply_cloth_affix(object ob, mapping affix)
{
    string name, prefix, suffix, k, pcolor, scolor;
    int value, v;
    mapping apply, part;

    if( !objectp(ob) ) return;
    init_cloth_tables();
    if( !mapp(affix) ) affix = ([]);
    if( !mapp(ob->query("cloth_base_apply")) )
        ob->set("cloth_base_apply", copy(ob->query("apply_armor/cloth")));
    if( !stringp(ob->query("cloth_base_name")) ) ob->set("cloth_base_name", ob->query("name"));
    if( !ob->query("cloth_base_value") ) ob->set("cloth_base_value", ob->query("value"));

    prefix = affix["prefix"];
    suffix = affix["suffix"];
    if( !stringp(prefix) || undefinedp(cloth_prefix_tab[prefix]) ) prefix = 0;
    if( !prefix || !stringp(suffix) || undefinedp(AFFIX_SUFFIX[suffix]) ) suffix = 0;

    apply = copy(ob->query("cloth_base_apply"));
    value = ob->query("cloth_base_value");
    name = ob->query("cloth_base_name");
    foreach(part in ({ prefix ? cloth_prefix_tab[prefix] : 0, suffix ? AFFIX_SUFFIX[suffix] : 0 })) {
        if( !mapp(part) ) continue;
        foreach(k, v in part) apply[k] += v;
        value = value * 3 / 2;
    }
    if( prefix ) {
        pcolor = AFFIX_COLOR && !undefinedp(cloth_color_tab[prefix]) ? cloth_color_tab[prefix] : 0;
        scolor = suffix ? affix_color(suffix) : 0;
        if( suffix )
            name = (pcolor ? pcolor + prefix + NOR : prefix)
                 + (scolor ? scolor + suffix + "衣" + NOR : suffix + "衣");
        else
            name = pcolor ? pcolor + prefix + "衣" + NOR : prefix + "衣";
    }
    ob->set("name", name);
    ob->set("value", value);
    ob->set("apply_armor/cloth", apply);
    if( prefix ) ob->set("affix", suffix ? ([ "prefix": prefix, "suffix": suffix ]) : ([ "prefix": prefix ]));
    else ob->set("affix", ([]));
}

// 繡花擲到詞綴：一個前綴，再 20% 機率加一個後綴。
void roll_cloth_affix(object ob)
{
    string *pre, *suf;
    mapping affix = ([]);

    init_cloth_tables();
    pre = keys(cloth_prefix_tab);
    suf = keys(AFFIX_SUFFIX);
    affix["prefix"] = pre[random(sizeof(pre))];
    if( random(100) < 20 ) affix["suffix"] = suf[random(sizeof(suf))];
    apply_cloth_affix(ob, affix);
}
