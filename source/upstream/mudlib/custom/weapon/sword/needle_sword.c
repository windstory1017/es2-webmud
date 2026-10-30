/* 雙鬢鋏 — ES2 原始武器資料（名稱、敘述）；數值與射針特攻照水嵐縣設計表。密林賊帥拿。 */
#include <ansi.h>
#include <weapon.h>

inherit F_SWORD;

void create()
{
    set_name("\x1b[1;37m雙鬢鋏\x1b[m", ({ "needle sword", "sword" }));
    set_weight(7500);   // 重量同長劍
    // 傷害照短劍的數值（obj/area/obj/shortsword.c 的 setup_sword(1, 15, 40, 1)）。
    init_damage(1, 15, 40, 1, "sword");
    init_damage(1, 15, 40, 1, "secondhand sword");

    if( !clonep() ) {
        set("wield_as", ({ "sword", "secondhand sword" }));
        set("unit", "把");
        set("value", 40000);
        set("long",
            "一把雪白的短劍，劍柄留有兩條相當長的白色絲帶。\n");
        // 數值改照水嵐縣設計表（賊帥拿；使用者指示，2026-10-30：新數值、舊敘述）。
        set("apply_weapon/sword", ([
            "attack": 10,
            "damage": 5,
            "str": 1,
        ]));
        set("apply_weapon/secondhand sword", ([
            "attack": 10,
            "damage": 5,
            "str": 1,
        ]));
    }
    setup();
}

/* 特攻（設計表）：出招落空時 50% 從劍鍔射出細針，傷對手的氣 10 點。 */
void miss_ob(object me, object victim)
{
    if( random(100) >= 50 ) return;
    message_vision(HIW "$N手腕一抖，" + name() + "的劍鍔裡驟然射出一蓬細針，直取$n！\n" NOR, me, victim);
    victim->consume_stat("kee", 10, me);
}
