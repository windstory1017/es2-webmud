// /d/swaylan/lunmay/npc/stake.c — 冷梅莊 NPC：假人（照振武軍營的假人，敘述改成冷梅莊的）。

#include <npc.h>
#include <ansi.h>
inherit F_VILLAGER;

void create()
{
    set_name("假人", ({"target stake","stake","假人"}));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("civilized", 1);
    set("training_dummy", 1);
    set("long", "一具用粗木紮成的假人，牢牢釘在地上，身上佈滿了深淺不一的劍痕，是冷梅莊弟子平日練劍用的。\n");
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
