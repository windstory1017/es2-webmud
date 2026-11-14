// /d/fengcheng/npc/gate_guard.c — 風城 NPC：將軍府門衛（H14）。
// 沒猜對小巷小孩的字謎（quest/fengcheng_riddle）的人，不讓往北進內院廣場；攔人的判斷在 general_gate.c。

#include <npc.h>

inherit F_SOLDIER;

void create()
{
    seteuid(getuid());
    set_name("將軍府門衛", ({ "guard" }));
    set_race("human");
    set_class("soldier");
    set_level(30);
    set("gender", "male");
    set("age", 30);
    set("gate_guard", 1);
    set("long", "將軍府大門前的守衛，一身鐵甲擦得雪亮，手按刀柄，目光銳利地打量著每一個走近的人。\n");
    set_skill("blade", 80);
    set_skill("dodge", 60);
    set_skill("parry", 60);
    set("vendetta_mark", "authority");
    set_power("B");
    setup();
    carry_object(STOCK_ARMOR("cloth"))->wear();
    carry_object(STOCK_WEAPON("broadsword"))->wield();
}

// 攔下想往北走的人。
void warn(object who)
{
    message_vision("$N把$n攔了下來。\n", this_object(), who);
}

int accept_fight(object ob)
{
    command("emote 瞪了瞪眼﹐說道﹕滾開滾開﹗少在這裡囉唆﹗");
    return 0;
}
