// /feature/npc/dummy.c — 練武假人（木樁、石獅子之類的死物）共用基底。
// 設計文件：docs/NPC_DUMMY.md
//
// 假人是死物：不還手、不閃避、不損壞，也沒有生命現象。
// 不會中毒、受傷、昏倒、甦醒、死亡，不吃不喝、不疲勞、不回血，
// 心跳整個關掉（被戰鬥或動作重新打開時，下一拍馬上再關掉）。
// 挨打時仍回傳正的傷害值，讓打中假人的修練經驗照常累計。
//
// 用法：
//   inherit "/feature/npc/dummy";
//   void create() { set_name(...); ...; setup(); }
// 不需要自己再寫 accept_fight / receive_damage / die 這些覆寫。
// 毒與狀態（set_condition）由 /feature/char/condition.c 依 training_dummy 擋下。
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

/* 假人從不還手。 */
void fight_ob(object ob) { }
void kill_ob(object ob) { }

/* 假人從不閃避或招架。 */
int defend(int ability, int strength, object from) { return 0; }
int absorb(int ability, int strength, object from) { return 0; }
int resist_damage(int damage, object from_ob) { return 0; }

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
