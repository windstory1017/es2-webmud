// /d/qiaoyin/npc/crone.c — 喬陰縣 NPC：老乞婆（依使用者設計表，2026-10-16）。
// 瑯夷派弟子把古錢交給她，會學到殺手本能（使用者定案，2026-11-06；
// 見 docs/martial_arts/瑯夷派_瑯夷派內功.md）。

#include <ansi.h>
#include <npc.h>
inherit F_VILLAGER;

void create()
{
    seteuid(getuid());
    set_name("老乞婆", ({ "crone" }));
    set_race("human");
    set_class("thief");
    set_level(3);
    set("gender", "female");
    set("age", 70);
    set("long", @LONG
一位老態龍鍾的婆婆，正蜷縮著身子窩在一旁，但你不知為何總覺得她的注意力全在你身上。
LONG
    );
    set_power("C");
    setup();
    carry_object("/obj/cloth")->wear();
}

// ------------------------------------------------------------------
// 古錢：瑯夷派弟子、還不會殺手本能的才收；兩句敘述間隔 1～2 回合（一回合兩秒），
// 第二句時學到殺手本能（技能經驗 100）。其他人給古錢她不收。
// ------------------------------------------------------------------

#define BUSY "longye/crone"

int accept_object(object who, object ob)
{
    if( !ob->query("thief_token") ) return 0;
    if( query_temp(BUSY) ) return 0;
    if( who->query("custom_faction") != "thief.longye" ) return 0;
    if( who->query_skill("killerhood", 1) || who->query_learn("killerhood") ) return 0;

    set_temp(BUSY, 1);
    call_out("pocket", 1, who);   // 等「你拿出古錢交給老乞婆」那行先出現
    return 1;
}

void pocket(object who)
{
    message_vision(CYN "$N接過古錢﹐瞇著眼翻來覆去看了兩回﹐不動聲色地揣進了懷裡。\n" NOR,
        this_object(), 0, 1);
    call_out("poke", (1 + random(2)) * 2, who);
}

void poke(object who)
{
    delete_temp(BUSY);
    if( !objectp(who) || environment(who) != environment() ) return;
    message_vision(CYN "$N身形一晃﹐不知何時已繞到$n身後﹐枯瘦的手指在$n背心輕輕一戳﹐"
        "$n頓時驚出一身冷汗。\n" NOR, this_object(), who, 1);
    if( !who->query_learn("killerhood") )
        who->improve_skill_exact("killerhood", 100);
}
