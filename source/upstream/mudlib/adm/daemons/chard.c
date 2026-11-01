/*---
description: Character setup daemon, used to setup characters and create corpse/mist/ghost.
author: Annihilator <taedlar@gmail.com>
---*/
#pragma save_binary

#include <ansi.h>
#include <login.h>
#include <statistic.h>

inherit F_CLEAN_UP;

private void create() {
  seteuid (getuid());
}

/** Setup character's basic attributes, statistics, and hit-points according to
 *  character's race, class, and life form. This function is called by character object during setup.
 */
void setup_char (object ob) {
    mapping apply;

    // assign default race, class, level if not set.
    if (!ob->query_race())
        ob->set_race ("human");
    if (!ob->query_class())
        ob->set_class ("commoner");
    if (!ob->query_level())
        ob->set_level (1);

    // assign statistics and attributes if not set.
    ob->init_statistic();
    ob->init_attribute();

    // initialize apply, which is used to temporarily modify character's attributes and statistics.
    if (mapp (apply = ob->query("perm_apply")))
        ob->set_temp ("apply", copy(apply));
    else
        ob->set_temp ("apply", ([]));

    // setup hit-points, which regenerate according to constitution.
    ob->set_stat_maximum ("HP", ob->query_attr("con") * 5);
    ob->set_stat_regenerate ("HP", TYPE_HEALTH);

    // setup fatigue, which decays according to character's life form.
    ob->set_stat_maximum ("fatigue", 100);
    ob->set_stat_effective ("fatigue", 100);
    if (!ob->query_stat_current("fatigue"))
        ob->set_stat_current ("fatigue", 0);
    ob->set_stat_regenerate ("fatigue", TYPE_WASTING);

    // setup statistics and regenerate type according to character's life form.
    switch (ob->query("life_form"))
    {
    case "ghost":
        ob->set_temp ("apply/vision_of_ghost", 1);
        ob->delete_stat ("kee");
        ob->delete_stat ("food");
        ob->delete_stat ("water");
        ob->set_stat_regenerate ("gin", TYPE_WASTING);
        ob->set_stat_regenerate ("sen", TYPE_WASTING);
        break;
    case "undead":
        ob->set_temp ("apply/vision_of_ghost", 1);
        ob->delete_stat ("sen");
        ob->set_stat_regenerate ("gin", TYPE_HEALTH);
        ob->set_stat_regenerate ("kee", TYPE_HEALTH);
        break;
    case "living":
    default:
        ob->set_stat_regenerate ("gin", TYPE_HEALTH);
        ob->set_stat_regenerate ("kee", TYPE_HEALTH);
        ob->set_stat_regenerate ("sen", TYPE_HEALTH);
        break;
    }

    // delegate to race/class daemon for further setup.
    RACE_D(ob->query_race())->setup (ob);
    CLASS_D(ob->query_class())->setup (ob);

    // 玩家每次登入依目前的職業公式重算升級門檻，公式調整後既有角色也會套用。
    if (userp(ob)) {
        ob->delete ("target_score");
        CLASS_D(ob->query_class())->initialize (ob);
    }

    // Food/water's maximums are only established just above, by the race
    // daemon (e.g. humanoid.c's setup()) -- set_stat_regenerate() silently
    // no-ops when a stat has no maximum yet, so registering food/water here
    // (previously done above, before the race daemon ran) could never
    // actually take effect, and they never wasted away over time. Ghosts
    // delete_stat() food/water above, which leaves their maximum at 0 too,
    // so this remains a safe no-op for them without needing a special case.
    // Wizards are immortal and don't need to eat or drink.
    ob->set_stat_regenerate ("food", wizardp(ob) ? TYPE_STATIC : TYPE_WASTING);
    ob->set_stat_regenerate ("water", wizardp(ob) ? TYPE_STATIC : TYPE_WASTING);

    ob->add_path ("/daemon/race/" + ob->query_race() + "/");
    ob->add_path ("/daemon/class/" + ob->query_class() + "/");
}

varargs object make_corpse (object victim, object killer) {
    int i, k, negative_fame;
    object corpse, inv, police1, police2;
    string corpse_ob;
    mapping bounty;
    string score;
    int amount;

    // wizards never leave a corpse.
    if (wizardp (victim))
        return 0;

    // notify inventory item to save data if nessessary.
    all_inventory(victim)->owner_is_killed (killer);

    // check life form.
    switch (victim->query("life_form"))
    {
    case "ghost":
        message ("vision", victim->name() + "化成一陣煙，消失了 ...\n",
            environment(victim), victim);
        return 0;
    case "living":
    default:
        all_inventory()->add ("sinister", 1);
    }

    // check if using custom corpse object.
    corpse_ob = victim->query("corpse_ob");
    if (!corpse_ob)
        corpse_ob = CORPSE_OB;

    // create corpse object and set it up.
    seteuid (MUDLIB_UID);
    catch (corpse = new(corpse_ob));
    seteuid (getuid());

    corpse->set_name (victim->name(1) + "的屍體",
        "corpse of " + capitalize (victim->query("id")), "corpse" );
    corpse->set ("long", victim->long()
        + "然而﹐" + gender_pronoun(victim->query("gender")) 
        + "已經死了﹐只剩下一具屍體靜靜地躺在這裡。\n");
    corpse->set ("age", victim->query("age"));
    corpse->set ("gender", victim->query("gender"));
    corpse->set ("victim_name", victim->name(1));
    // 玩家的屍體做記號（腐蝕之手等不能用在玩家屍體上）。
    if (userp (victim))
        corpse->set ("player_corpse", 1);

    // relate victim with hir corpse.
    victim->set_temp ("corpse", corpse);

    // move the corpse to the environment of the victim, and start decaying.
    corpse->set_weight (victim->query_weight());
    corpse->set_max_encumbrance (victim->query_max_encumbrance());
    corpse->move (environment(victim));
    corpse->start_decay (userp(victim) ? 90 : 60);

    // transfer victim's inventory into corpse.
    foreach (inv in all_inventory(victim))
        inv->move (corpse);
        
    // handle the killer.
    if (!killer)
        killer = victim->last_damage_giver();

    if (objectp (killer)) {
        // Player-killing
        if (userp (victim) && userp (killer) && (victim != killer) && (victim->query_level() > 1) ) {
            killer->gain_score ("mortal sin",
                victim->query_level() * victim->query_level() * 10);
            // 如果被害人素行良好, 殺人者折損自身聲望及武林聲望, 殺人者
            // 若是小偷, 則略增黑道聲望 -dragoon
            negative_fame = victim->query_score("reputation") + 
                victim->query_level() * victim->query_level() -
                victim->query_score("mortal sin") / 15;

            if( negative_fame > 0 ) {
                k = random(negative_fame);
                // 殺人不再扣聲望與武林聲望（使用者定案，2026-10-16）。
                // 小偷略增黑道聲望
                if( killer->query("class") == "thief" )
                    killer->gain_score("negative fame", k/10);
            }
        }

        // if pker die, delete mark about pker -dragoon
        if( victim->query("last_pk_time")
        || victim->query("pker")
        || victim->query("pk_record") ) {
            victim->delete("last_pk_time");
            victim->delete("pker");
            victim->delete("pk_record");
        }
        // set marks on PKer -dragoon
        if( userp(killer) && userp(victim) ) {
            // 若是此人曾殺過人, 且時間少於某段時間, 則記錄上加一筆, 否則, 
            // 便重設時間記錄
            // 加上設限: 必需lv>1才會增加, 以防神風特攻隊以另類方式 pk
            if( killer->query("last_pk_time")
            && time() - (int)killer->query("last_pk_time") < 60 * 60 ) {
                if( victim->query("level") > 1 )
                    killer->add("pk_record", 1); }
            else {  killer->set("pk_record", 1); }
            // 設上最近一次殺人的時間
            killer->set("last_pk_time", time());
            // 殺人數高於3開始有機會出現捕頭, 另外加上治安指數
            // 目前設定: 治安普通, 命案發生有三成機率出現捕頭
            if( killer->query("pk_record") > 3 + random(12) 
            && random(10) > 6 ) {
                if( killer->query("pk_record") > 15 + random(10) ) {
                    write("兩個捕頭遠遠的聽見慘叫，迅速的奔了過來!!\n");
                    police1 = new("/d/domain/npc/police_leader");
                    police1->move(environment(killer)); }
                else write("捕頭遠遠的聽見慘叫，迅速的奔了過來!!\n");
                police2 = new("/d/domain/npc/police");
                police2->move(environment(killer));
            }		
        }

        // Register vendetta marks if any.
        if( victim->query("vendetta_mark") )
            killer->add("vendetta/" + victim->query("vendetta_mark"), 1);

        // Killing humanoid creatures adds kills statistic
        if( victim->query("humanoid") )
            killer->gain_score("kills", 1);

        // Gain other bounty if any.
        if( mapp(bounty = victim->query("bounty")) )
            foreach(score, amount in bounty)
                killer->gain_score(score, amount);

        // Finally, let class daemon check this kill.
        CLASS_D(killer->query_class())->kill_check(killer, victim, corpse);
    }        

    return corpse;
}

int make_mist (object victim) {
    LOGIN_D->reincarnate (victim);
    return 1;
}

/*  生命型態轉換：生物 -> 鬼魂
 */
int make_ghost (object victim) {
    object corpse;
    int fatigue;

    if( victim->query("life_form") != "living" )
        error ("鬼魂只能由生物轉換而成。\n");

    corpse = victim->query_temp("corpse");
    victim->set ("life_form", "ghost");
    victim->set ("living", ([
        "kee" : victim->query_stat_maximum("kee"),
    ]));

    /* 死亡時疲勞超過 80：復活後精氣神只恢復 (100 - (疲勞 - 80) x 5)%（使用者定案，2026-10-29）。 */
    fatigue = victim->query_stat("fatigue");
    if (fatigue > 80) victim->set ("revive_pct", 100 - (fatigue - 80) * 5);
    else victim->delete ("revive_pct");

    victim->supplement_stat("HP", 9999);
    victim->supplement_stat("gin", 9999);
    victim->supplement_stat("sen", 9999);
    victim->clear_statistic_flags();

    victim->clear_temp_dbase();
    victim->setup();

    /* 重新設定屍體的連結，因為 clear_temp_dbase() 會清掉 */
    if (objectp(corpse))
        victim->set_temp ("corpse", corpse);
    return 1;
}

int make_living (object ob) {
    object corpse;
    mapping living_stat;
    string stat;
    int max, pct, cur;

    living_stat = ob->query ("living");
    corpse = ob->query_temp ("corpse");

    /* 恢復生物的基本數值 */
    if (mapp (living_stat))
        foreach (stat, max in living_stat) {
            ob->set_stat_maximum (stat, max);
            ob->set_stat_effective (stat, max);
            ob->set_stat_current (stat, max);
        }

    /* 屍體已經消失才復活：精、氣、神的最大值超過 100 的，各扣 1%（使用者核准，原本只扣氣 10%）。 */
    if (!corpse)
        foreach (stat in ({ "gin", "kee", "sen" })) {
            max = ob->query_stat_maximum (stat);
            if (max <= 100) continue;
            max -= max / 100;
            ob->set_stat_maximum (stat, max);
            if (ob->query_stat_effective (stat) > max) ob->set_stat_effective (stat, max);
            if (ob->query_stat_current (stat) > max) ob->set_stat_current (stat, max);
        }

    ob->set ("life_form", "living");
    ob->delete ("living");

    ob->clear_temp_dbase();
    ob->setup();

    /* 死亡時疲勞過高：精、氣、神只恢復一部分，最小為 1（最大值不變，之後可以慢慢補回）。 */
    if (!undefinedp (ob->query ("revive_pct"))) {
        pct = ob->query ("revive_pct");
        ob->delete ("revive_pct");
        if (pct < 100)
            foreach (stat in ({ "gin", "kee", "sen" })) {
                cur = ob->query_stat_maximum (stat) * pct / 100;
                if (cur < 1) cur = 1;
                ob->set_stat_current (stat, cur);
            }
    }
    return 1;
}

void hint_user_race (string race) {
    switch (race) {
        case "human":
            write (CHA(20) + "人類是最常見的種族，沒有特別的優缺點，適合初學者。");
            break;
        case "avatar":
            write (CHA(20) + "人類族與人類外觀相近，但具有更整齊的先天屬性與 radiate 種族能力。");
            break;
        case "blackteeth":
            write (CHA(20) + "黑齒擅長以 gnaw 咬擊敵人並留下持續性的毒傷。");
            break;
        case "yenhold":
            write (CHA(20) + "厭火具有火焰防禦，並能以 breathe 向同房敵人噴吐火焰。");
            break;
        case "jiaojao":
            write (CHA(20) + "焦僥警覺與行動敏捷，能在戰鬥中以 hide 脫離並原地躲藏。");
            break;
        case "woochan":
            write (CHA(20) + "無腸不需要食物，並能使用 replete 恢復受損的精氣神格子。");
            break;
        case "dingling":
            write (CHA(20) + "釘靈天生強健，能以 hoof 踢翻敵人，令其數回合無法行動。");
            break;
        case "rainner":
            write (CHA(20) + "雨師妾天生靈性過人，升級時有機會得到小蛇，以 feed 餵養鮮血讓牠們成長。");
            break;
        case "headless":
            write (CHA(20) + "刑天無首，以乳為目、以臍為口，天生戰士，手持斧頭時能以五種戰舞提升或恢復戰力。");
            break;
        case "malik":
            write (CHA(20) + "巫首悟性與靈性極高，天生防禦力強，沒有特殊種族能力。");
            break;
        case "yaksa":
            write (CHA(20) + "夜叉天生陰陽眼，能以 devour 吞食鬼魂，吞食玩家的鬼魂可以永久提升精氣神。");
            break;
        case "ashura":
            write (CHA(20) + "阿修羅天生好戰，遇到 NPC 時常會忍不住出手，定力越高越能克制。");
            break;
    }
}
