// /d/baihua/npc/stone_lion.c — 百花村：石獅子（依使用者設計表，2026-10-20）。
// 比照振武軍營假人（training_dummy）：不還手、不損壞，被擊中的敘述不提部位。
#include <npc.h>
#include <ansi.h>
inherit F_VILLAGER;

void create()
{
    set_name("石獅子", ({"stone lion","lion","石獅子"}));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("civilized", 1);
    set("training_dummy", 1);
    set("long", "一尊蹲坐在廟門口的石獅子，石身佈滿青苔與裂紋，鬃毛的雕紋卻依舊清晰。它張著大口、瞪著圓眼，神態威武，不時有路過的人朝它比劃幾下拳腳，它也只是默默地承受著。
");
    setup();
}

int accept_fight(object who) { return 1; }

/* 假人是死物：被 kill 時不會喊救命。 */
void accept_kill(object ob) { }

/* Training target never retaliates. */
void fight_ob(object ob) { }
void kill_ob(object ob) { }

/* Training target never dodges or parries. */
int defend(int ability, int strength, object from) { return 0; }
int absorb(int ability, int strength, object from) { return 0; }
int resist_damage(int damage, object from_ob) { return 0; }

/* Return a positive hit so hit-triggered learned EXP is awarded, but do not
 * change kee/HP/effective stats. */
varargs int receive_damage(int damage, object from, object attacker)
{
    if( damage < 1 ) damage = 1;
    return damage;
}

void die() { }
