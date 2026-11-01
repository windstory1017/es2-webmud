/*---
description: NPC 基本技能 —— 依職業、等級、門派自動算出 NPC 的技能等級。
custom: NEW. WebMUD NPC 技能設計（使用者設計，2026-10-14）：技能只看等級，不受強度 C/B/A/S 影響。
---*/

// 用法：有 set_power() 的 NPC 在 setup() 時自動套用，不必另外寫。
//
// 規則（docs/CUSTOM_NPC_SKILL.md）：
//   技能等級 = 等級 × 係數，最高 200。
//     主修 ×3、通用 ×2、輔助 ×1.5。
//   平民：不設技能。
//   其他職業通用：dodge、parry、unarmed（×2）。
//   手上武器的技能一律算主修（裝備後才知道，見 apply_weapon_skills()）。
//   各職業主修／輔助見下方 class_skill。
//   門派（custom_faction）：達到內功門檻等級時學會該門派內功（主修），
//     並把 force 對應到該內功；門派武功清單之後補上（sect 表的 "arts"）。
//   create() 裡手動 set_skill()/map_skill() 的項目保留不覆蓋；手寫了內功等級
//   但沒寫對應的，仍會把內功對應到門派內功。

#define SKILL_CAP       200

// 係數（百分比）
#define MAIN_PCT        300
#define COMMON_PCT      200
#define AUX_PCT         150

private string *common_skill = ({ "dodge", "parry", "unarmed" });

// 各職業的主修與輔助技能。平民不在表上（不設技能）。
// "common" 為 0 表示該職業不套用通用技能。
private mapping class_skill = ([
    "fighter":   ([ "main": ({ }),                                      "aux": ({ }) ]),
    "soldier":   ([ "main": ({ "berserk" }),                            "aux": ({ }) ]),
    "thief":     ([ "main": ({ "backstab", "killerhood" }),             "aux": ({ "sneak", "dagger" }) ]),
    "taoist":    ([ "main": ({ "spells" }),                             "aux": ({ }) ]),
    "scholar":   ([ "main": ({ "dodge", "literate", "archaic attainment" }), "aux": ({ }) ]),
    "monk":      ([ "main": ({ "magic" }),                              "aux": ({ }) ]),
    "alchemist": ([ "main": ({ "magic" }),                              "aux": ({ }) ]),
]);

// 門派表，以 custom_faction 為索引（與玩家拜師後的記號相同）。
// 依使用者提供的「武功對照表」（docs/martial_arts/門派武功對照表.md，2026-10-14）。
//   name        門派名稱
//   force       門派內功；force_level 為學會內功所需的人物等級（0 表示沒有自動學會的內功）
//   arts        門派武功：({ ([ "skill": 武功, "map": 對應的基本技能或 0, "level": 人物等級門檻,
//                              "req": ([ 前置武功: 至少幾級 ]) ]) })
//               「拜師後」為等級 1；「解謎」取得的武功不會自動給門派 NPC，只記在文件裡。
//   前置條件（使用者規則，2026-10-14）：NPC 只要能學到前置武功，就視為該武功已練滿，
//               例如傲梅暗劍訣要 LV30＋寒梅心法 100，寒梅心法 LV15 就學得到，所以 LV30 即可學。
//   同一個基本技能有多套武功時，以先列的那套為主（後面的只學會、不對應）。
//   還沒有武功檔的只給名稱與等級，不做對應（對應到不存在的武功會讓戰鬥出錯）。
private mapping sect = ([
    "fighter.lunmay": ([
        "name":        "冷梅莊",
        "force":       "hainmay force",
        "force_level": 15,
        "arts":        ({
            ([ "skill": "mayin", "map": "dodge", "level": 1 ]),
            ([ "skill": "seven", "map": "unarmed", "level": 1 ]),
            ([ "skill": "lunmay", "map": "sword", "level": 1 ]),
            ([ "skill": "lunmay", "map": "parry", "level": 1 ]),
            ([ "skill": "advance_lunmay", "map": "secondhand sword", "level": 30, "req": ([ "hainmay force": 100 ]) ]),
            ([ "skill": "advance_lunmay", "map": "secondhand dagger", "level": 30, "req": ([ "hainmay force": 100 ]) ]),
        }),
    ]),
    "fighter.tiger": ([
        "name":        "虎刀門",
        "force":       "tiger force",
        "force_level": 15,
        "arts":        ({
            ([ "skill": "ksanmando", "map": "blade", "level": 1 ]),
            ([ "skill": "tiger-blade", "map": "twohanded blade", "level": 1, "req": ([ "tiger force": 30 ]) ]),
            ([ "skill": "tiger-steps", "map": "dodge", "level": 1 ]),
        }),
    ]),
    "fighter.fonxan": ([
        "name":        "封山派",
        "force":       "fonxan force",
        "force_level": 15,
        "arts":        ({
            ([ "skill": "chaos-steps", "map": "dodge", "level": 1 ]),
            ([ "skill": "fonxansword", "map": "sword", "level": 1 ]),
        }),
    ]),
    "fighter.chin": ([
        "name":        "哭笑門",
        "force":       "huge force",
        "force_level": 11,
        "arts":        ({
            ([ "skill": "chitensword", "map": "sword", "level": 1 ]),
            ([ "skill": "chin_staff", "map": "staff", "level": 1 ]),
            ([ "skill": "chin_staff", "map": "twohanded staff", "level": 1 ]),
        }),
    ]),
    "fighter.snow": ([
        "name":        "雪吟莊",
        "force":       "tidy force",
        "force_level": 5,
        "arts":        ({
        }),
    ]),
    "fighter.ancient": ([   // 內功迴風拂柳心訣為解謎取得
        "name":        "古劍派",
        "force":       0,
        "force_level": 0,
        "arts":        ({
        }),
    ]),
    "scholar.sky": ([
        "name":        "玄天派",
        "force":       "sky force",
        "force_level": 5,
        "arts":        ({
            ([ "skill": "advance-sky sword", "map": "sword", "level": 1 ]),
        }),
    ]),
    "thief.longye": ([
        "name":        "瑯夷派",
        "force":       "longyeforce",
        "force_level": 5,
        "arts":        ({
        }),
    ]),
    "thief.hidden": ([
        "name":        "隱教",
        "force":       "lightswift",
        "force_level": 1,
        "arts":        ({
            ([ "skill": "lightswift", "map": "dodge", "level": 1 ]),
        }),
    ]),
    "taoist.fire": ([   // 對照表未列火術，比照其他三派補上（使用者確認）
        "name":        "朱衣派",
        "force":       "sun force",
        "force_level": 15,
        "arts":        ({
            ([ "skill": "taoism-fire", "map": "spells", "level": 1 ]),
            ([ "skill": "eight_steps", "map": "dodge", "level": 1 ]),
        }),
    ]),
    "taoist.freeze": ([
        "name":        "素衣派",
        "force":       "polar-silkworm force",
        "force_level": 15,
        "arts":        ({
            ([ "skill": "taoism-freeze", "map": "spells", "level": 1 ]),
        }),
    ]),
    "taoist.storm": ([
        "name":        "玄衣派",
        "force":       "dao force",
        "force_level": 20,
        "arts":        ({
            ([ "skill": "taoism-storm", "map": "spells", "level": 1 ]),
        }),
    ]),
    "taoist.thunder": ([
        "name":        "紫衣派",
        "force":       "nine-dragon force",
        "force_level": 10,
        "arts":        ({
            ([ "skill": "taoism-thunder", "map": "spells", "level": 1 ]),
            ([ "skill": "reflecting star sword", "map": "sword", "level": 1 ]),
            ([ "skill": "magic steps", "map": "dodge", "level": 1 ]),
        }),
    ]),
    "taoist.fog": ([
        "name":        "幻霧觀",
        "force":       0,
        "force_level": 0,
        "arts":        ({
            ([ "skill": "taoism of darkness", "map": "spells", "level": 1 ]),
            ([ "skill": "mao-shan magic", "map": "magic", "level": 1 ]),
            ([ "skill": "mao-shan mysticism", "map": "magic", "level": 1, "req": ([ "taoism of darkness": 100 ]) ]),
        }),
    ]),
    "taoist.wind": ([
        "name":        "隱風觀",
        "force":       0,
        "force_level": 0,
        "arts":        ({
            ([ "skill": "taoism of nature", "map": "spells", "level": 1 ]),
            ([ "skill": "mao-shan magic", "map": "magic", "level": 1 ]),
            ([ "skill": "taoism of dunja", "map": "magic", "level": 30, "req": ([ "taoism of nature": 90 ]) ]),
        }),
    ]),
    "taoist.lingyun": ([
        "name":        "靈雲觀",
        "force":       0,
        "force_level": 0,
        "arts":        ({
            ([ "skill": "taoism of purify", "map": "spells", "level": 1 ]),
            ([ "skill": "mao-shan magic", "map": "magic", "level": 1 ]),
            ([ "skill": "mao-shan lightning", "map": "magic", "level": 1, "req": ([ "taoism of purify": 91 ]) ]),
        }),
    ]),
    // 武陀灸堂（docs/martial_arts/武陀灸堂_門派設計.md，2026-10-27）。
    // 引源針法、三自回針法要經脈穴道之學 80，NPC 一併給經脈穴道之學。
    "alchemist.wuto": ([
        "name":        "武陀灸堂",
        "force":       0,
        "force_level": 0,
        "arts":        ({
            ([ "skill": "amazing-needle", "map": "cauterization", "level": 1 ]),
            ([ "skill": "wuto-needle", "map": "cauterization", "level": 1 ]),
            ([ "skill": "kee anatomy", "map": 0, "level": 1 ]),
            ([ "skill": "potency-needle", "map": "cauterization", "level": 1, "req": ([ "kee anatomy": 80 ]) ]),
            ([ "skill": "recovery-needle", "map": "cauterization", "level": 1, "req": ([ "kee anatomy": 80 ]) ]),
        }),
    ]),
]);

void create() { seteuid(getuid()); }

int calc_level(int lv, int pct)
{
    int val;

    if( lv < 1 ) lv = 1;
    val = lv * pct / 100;
    if( val < 1 ) val = 1;
    return val > SKILL_CAP ? SKILL_CAP : val;
}

// 武功檔是否存在。找不到時 DAEMON_D 會傳回它自己，它沒有 valid_enable()。
int has_art(string skill)
{
    object ob;
    return objectp(ob = SKILL_D(skill)) && function_exists("valid_enable", ob) ? 1 : 0;
}

mapping query_sect(string faction) { return mapp(sect[faction]) ? copy(sect[faction]) : 0; }

private void put(mapping m, string skill, int val)
{
    if( val > m[skill] ) m[skill] = val;
}

// 依職業、等級、門派算出應有的技能（不含武器技能）。
// 傳回 ([ "skills": ([ 技能: 等級 ]), "map": ([ 基本技能: 對應武功 ]) ])
mapping calc_skills(string cls, int lv, string faction)
{
    mapping skills, maps, cs, s;
    string sk;

    skills = ([]);
    maps = ([]);
    if( !mapp(cs = class_skill[cls]) )      // 平民或未知職業：不設技能
        return ([ "skills": skills, "map": maps ]);

    foreach( sk in common_skill ) put(skills, sk, calc_level(lv, COMMON_PCT));
    foreach( sk in cs["aux"] )    put(skills, sk, calc_level(lv, AUX_PCT));
    foreach( sk in cs["main"] )   put(skills, sk, calc_level(lv, MAIN_PCT));

    if( mapp(s = sect[faction]) ) {
        if( s["force"] && lv >= s["force_level"] ) {
            put(skills, "force", calc_level(lv, MAIN_PCT));
            put(skills, s["force"], calc_level(lv, MAIN_PCT));
            if( has_art(s["force"]) ) maps["force"] = s["force"];
        }
        foreach( mapping art in s["arts"] ) {
            if( lv < art["level"] ) continue;
            if( mapp(art["req"]) ) {
                // 前置武功只要學得到就視為練滿，不看實際等級。
                int ok = 1;
                foreach( string rq, int need in art["req"] )
                    if( skills[rq] < 1 ) ok = 0;
                if( !ok ) continue;
            }
            put(skills, art["skill"], calc_level(lv, MAIN_PCT));
            if( art["map"] ) {
                put(skills, art["map"], calc_level(lv, MAIN_PCT));
                // 同一個基本技能有多套武功時，以先列的為主。
                if( !maps[art["map"]] && has_art(art["skill"]) ) maps[art["map"]] = art["skill"];
            }
        }
    }
    return ([ "skills": skills, "map": maps ]);
}

// 套用到 NPC 身上。由 /std/char/npc.c 的 setup() 呼叫。
void apply_skills(object ob, mapping manual_skill, mapping manual_map)
{
    mapping r;
    string sk;
    int lv;

    if( !objectp(ob) ) return;
    if( !mapp(manual_skill) ) manual_skill = ([]);
    if( !mapp(manual_map) ) manual_map = ([]);

    r = calc_skills(ob->query_class(), ob->query_level(), ob->query("custom_faction"));
    foreach( sk, lv in r["skills"] )
        if( !manual_skill[sk] ) ob->set_skill(sk, lv);
    foreach( sk in keys(r["map"]) )
        if( !manual_map[sk] ) ob->map_skill(sk, r["map"][sk]);
}

// 手上武器的技能（主修）。NPC 在 setup() 之後才拿武器，所以晚一步套用。
void apply_weapon_skills(object ob, mapping manual_skill)
{
    mapping weapon;
    string sk;

    if( !objectp(ob) || !mapp(class_skill[ob->query_class()]) ) return;
    if( !mapp(manual_skill) ) manual_skill = ([]);
    if( !mapp(weapon = ob->query_temp("weapon")) ) return;

    foreach( sk in keys(weapon) ) {
        if( manual_skill[sk] ) continue;
        if( ob->query_skill(sk, 1) < calc_level(ob->query_level(), MAIN_PCT) )
            ob->set_skill(sk, calc_level(ob->query_level(), MAIN_PCT));
    }
}
