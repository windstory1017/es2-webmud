/*---
description: 茅山幻術【喚犬之術】召喚出的犬隻（使用者同意新增，2026-10-22）。
             跟著主人走、保護主人；至死方休；主人離開遊戲就散去。
             強度：野獸強度表的野獸技能、傷害加成 ×2，精、氣、神 ×3。
             設計見 docs/martial_arts/隱風觀_茅山幻術與遁甲書.md。
---*/
#include <ansi.h>
#include <npc.h>

object owner;
int strength;

void create()
{
    seteuid(getuid());
    set_name("黃狗", ({ "summoned dog", "dog" }));
    set_race("beast");
    set_class("commoner");
    set_level(1);
    set("long", "一條由符紙化成的犬隻﹐一雙眼睛緊盯著主人的敵人。\n");
    set("unit", "條");
    set("beast_actions", ({
        ([ "action": "$N齜牙低吼﹐撲上去往$n的$l狠狠咬落", "damage_type": "咬傷" ]),
        ([ "action": "$N繞著$n疾走半圈﹐冷不防竄起來抓向$n的$l", "damage_type": "抓傷" ]),
    }));
    setup();
}

/* 名稱依強度，同一階隨機一種，只有名字不同（使用者定案，2026-10-24）：
 * 1～3 黃狗、白狗、灰狗；4～6 青犬、赤犬；7～10 靈獒（紫紅或青）。 */
string dog_name(int level)
{
    string *names, *colors;
    int i;

    if( level >= 7 ) {
        names = ({ "靈獒", "靈獒" });
        colors = ({ MAG, CYN });
    } else if( level >= 4 ) {
        names = ({ "青犬", "赤犬" });
        colors = ({ BLU, RED });
    } else {
        names = ({ "黃狗", "白狗", "灰狗" });
        colors = ({ YEL, HIW, HIK });
    }
    i = random(sizeof(names));
    set("dog_kind", names[i]);      // 不含顏色的名字，召喚敘述合併計數用
    return colors[i] + names[i] + NOR;
}

string query_dog_kind() { return query("dog_kind"); }

/* 主人召喚時呼叫：套用野獸強度表，再把精氣神乘 3、野獸技能與傷害加成乘 2。 */
void set_summon(object who, int level)
{
    owner = who;
    strength = level;
    set_name(dog_name(level), ({ "summoned dog", "dog" }));
    RACE_D("beast")->set_strength(this_object(), level);
    foreach(string st in ({ "gin", "kee", "sen" })) {
        set_stat_maximum(st, query_stat_maximum(st) * 3);
        set_stat_effective(st, query_stat_maximum(st));
        set_stat_current(st, query_stat_maximum(st));
    }
    set_skill("beast", query_skill("beast", 1) * 2);
    set_temp("apply/damage", query_temp("apply/damage") * 2);
    set_leader(who);
    set("maoshan_owner", who->query("id"));
}

object query_owner() { return owner; }

/* 收回（重新召喚、主人離開）：化回符紙。 */
void dismiss()
{
    if( environment() )
        message_vision("$N抖了抖身子﹐化回一張焦黑的符紙飄落在地。\n", this_object());
    destruct(this_object());
}

/* 每回合：主人不在了就散去；跟不上主人就追過去；主人在打誰就一起撲上去。 */
void guard_owner()
{
    if( !objectp(owner) ) {
        dismiss();
        return;
    }
    if( !environment(owner) ) return;
    if( environment(owner) != environment() ) {
        move(environment(owner));
        message_vision("$N一路嗅著氣味追了上來﹐伏在$n腳邊。\n", this_object(), owner);
        return;
    }
    foreach(object ob in owner->query_enemy()) {
        if( !objectp(ob) || ob == this_object() || environment(ob) != environment() ) continue;
        if( !is_fighting(ob) ) {
            kill_ob(ob);
            ob->kill_ob(this_object());
        }
    }
}

static void heart_beat()
{
    ::heart_beat();
    if( this_object() && living(this_object()) ) guard_owner();
}

/* 被打死：化作青煙散去，不留屍體。 */
void die()
{
    message_vision("$N哀鳴一聲﹐化作一縷青煙散去。\n", this_object());
    destruct(this_object());
}
