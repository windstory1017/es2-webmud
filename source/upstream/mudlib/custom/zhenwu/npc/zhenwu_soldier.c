// /custom/zhenwu/npc/zhenwu_soldier.c
// NEW（振武軍營新增角色，非原始 ES2 資料，由專案成員指定人設）

#include <npc.h>
inherit F_SOLDIER;
void create()
{
    set_name("振武軍營士兵", ({"soldier", "振武軍營士兵"}));
    set_race("human");
    set_class("soldier");
    set_level(35);
    set("long", "振武軍營士兵是天朝帝國邊防最基層的步兵。他身穿粗糙皮甲，手執沉重長槍，雖沒有江湖大俠般的絕頂輕功，卻有一身扎實的軍旅外家拳腳，每日都在營邊巡邏警戒。\n");
    set_skill("berserk", 200);
    set("chat_chance_combat", 30);
    set("chat_msg_combat", ({ (: command, "berserk" :) }));
    set_power("C");
    setup();
    carry_object("/d/snow/npc/obj/clothboot")->wear();
    carry_object("/d/snow/npc/obj/helmet")->wear();
    carry_object("/d/snow/npc/obj/leather_armor")->wear();
    carry_object("/obj/area/obj/pike")->wield();
}
