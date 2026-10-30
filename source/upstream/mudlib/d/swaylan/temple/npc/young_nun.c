// /d/swaylan/temple/npc/young_nun.c — 寶蓮寺 NPC：小尼姑（依水嵐縣設計表）。會走動，但不離開寶蓮寺。

#include <npc.h>
#include <command.h>
inherit F_VILLAGER;

#define AREA_PREFIX     "/d/swaylan/temple/"
#define WANDER_INTERVAL 4       // 每 2 tick（4 秒）擲一次
#define WANDER_CHANCE   10      // 10% 機率移動

void create()
{
    seteuid(getuid());
    set_name("小尼姑", ({ "young nun", "nun" }));
    set_race("human");
    set_class("monk");
    set_level(5);
    set("gender", "female");
    set("age", 10);
    set("long", @LONG
一個年紀尚小的小尼姑，光溜溜的頭頂上點著戒疤，一身灰布僧衣洗得發白。她手裡捻著念珠，在寺裡忙進忙出，見了香客便雙手合十，怯生生地行個禮。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    set_power("C");
    setup();
    carry_object("/d/swaylan/temple/obj/rosary")->wear();
    if( clonep() ) call_out("wander", WANDER_INTERVAL);
}

// 只走往寶蓮寺內的出口。
void wander()
{
    object env;
    mapping exits;
    string *dirs = ({});

    call_out("wander", WANDER_INTERVAL);
    if( !living(this_object()) || is_fighting() || is_busy() ) return;
    if( !objectp(env = environment()) ) return;
    if( random(100) >= WANDER_CHANCE ) return;
    if( !mapp(exits = env->query("exits")) ) return;
    foreach( string dir, mixed dest in exits )
        if( stringp(dest) && strsrch(dest, AREA_PREFIX) == 0 )
            dirs += ({ dir });
    if( sizeof(dirs) )
        GO_CMD->main(this_object(), dirs[random(sizeof(dirs))]);
}
