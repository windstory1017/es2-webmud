// /d/kangping/npc/kid.c — 康平村 NPC：阿桂（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("阿桂", ({ "kid", "a-gui" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 10);
    set("long", @LONG
一個機靈的小男孩，臉蛋曬得紅通通的，見了外地人一點也不怕生，總是熱心地跑上前來，七嘴八舌地介紹村裡哪家的井水最甜、哪條路通到谷口，還不忘提醒你夜裡別往谷裡去。
LONG
    );
    set("chat_chance", 3);
    set("chat_msg", ({
        "阿桂提著水桶跑到井邊，踮起腳尖往井裡探了探。\n",
        "阿桂說道﹕谷裡有狼，天黑以後千萬別進去喔！\n",
        "阿桂對著你咧嘴一笑，露出缺了一顆的門牙。\n",
    }));
    set_power("C");
    setup();
}
