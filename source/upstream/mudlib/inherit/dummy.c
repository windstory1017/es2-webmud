// /inherit/dummy.c — 練武假人共用基底（測試版）。
// 設計文件：docs/NPC_DUMMY.md（「測試中的新基底 /inherit/dummy.c」一節）
// 以 /feature/npc/dummy.c 為底本，另外補上「完全不還手」與「偷襲必定成功」。
// 測試沒問題後，再把這些內容併回現有假人的繼承檔。
//
// 假人是死物：不還手、不閃避、不格擋、不損壞，也沒有生命現象。
// 不會中毒、受傷、昏倒、甦醒、死亡，不吃不喝、不疲勞、不回血，
// 心跳整個關掉（被戰鬥或動作重新打開時，下一拍馬上再關掉）。
// 挨打時仍回傳正的傷害值，讓打中假人的修練經驗照常累計。
// 掛上 training_dummy 記號後：
//   戰鬥系統必定命中、敘述不提部位與傷勢；
//   毒與狀態（set_condition）由 /feature/char/condition.c 擋下；
//   喚蟒之術、茅山陰魂附身照舊不能對假人施放。
//
// 用法：
//   inherit "/inherit/dummy";
//   void create() { set_name(...); ...; setup(); }
#include <npc.h>
#include <ansi.h>

void setup()
{
    set("training_dummy", 1);
    set("civilized", 1);
    ::setup();
    set_heart_beat(0);
}

/* 心跳：被重新打開時立刻關掉，不跑回血、狀態與昏迷的流程。 */
static void heart_beat() { set_heart_beat(0); }

int accept_fight(object who) { return 1; }

/* 假人是死物：被 kill 時不會喊救命。 */
void accept_kill(object ob) { }

/* 假人從不還手：不進入戰鬥，也不出手。
 * 修正還手漏洞：偷襲在「繞到背後」被察覺時，偷襲指令會直接叫目標出手一次，
 * 原本的基底沒有擋住 attack()，所以假人會打玩家一拳。 */
void fight_ob(object ob) { }
void kill_ob(object ob) { }
varargs int attack(object opponent) { return 0; }

/* 假人從不閃避或招架。 */
int defend(int ability, int strength, object from) { return 0; }
int absorb(int ability, int strength, object from) { return 0; }
int resist_damage(int damage, object from_ob) { return 0; }

/* 沒有警覺、沒有防禦、沒有應變：
 *   偷襲兩次察覺判定（警覺 + 隨機精）一定不會發現玩家；
 *   偷襲得手後的追擊判定（隨機防禦 < 殺手本能）只看玩家有沒有殺手本能；
 *   戰鬥時玩家選定假人出手，不會因為對方的應變而落空。 */
int query_ability(string application)
{
    switch( application ) {
    case "awarness":
    case "defense":
    case "wittiness":
        return 0;
    default:
        return ::query_ability(application);
    }
}

/* 精固定為 0：偷襲察覺判定裡的隨機精不會加分。
 * 假人不會昏倒（見下方），精為 0 不影響它。 */
int query_stat(string what)
{
    if( what == "gin" ) return 0;
    return ::query_stat(what);
}

/* 氣的上限固定為 0：死物沒有體力可言。戰鬥系統的 report_status() 與各技能報體力前
 * 都會先看氣的上限，是 0 就不報，所以任何技能打假人都不會出現體力敘述。 */
mixed query_stat_maximum(string what)
{
    if( what == "kee" ) return 0;
    return ::query_stat_maximum(what);
}

/* 回傳正的傷害值，讓命中時的修練經驗照常累計，但不扣任何狀態。 */
varargs int receive_damage(int damage, object from, object attacker)
{
    if( damage < 1 ) damage = 1;
    return damage;
}

/* 無生命體：任何來源都不能耗它的精氣神、體力、飲食，也不會疲勞。 */
varargs int consume_stat(string type, int damage, object who) { return 0; }
varargs int damage_stat(string type, int damage, object who) { return 0; }
int supplement_stat(string type, int heal) { return 0; }
int heal_stat(string type, int heal) { return 0; }

/* 不會昏倒、甦醒或死亡。 */
varargs void unconcious(string reason) { }
varargs void revive(int quiet) { }
void revive_faint() { }
void die() { }
