// /custom/wizroom/w/windstory/obj/stone_stake.c — 石製假人（測試用）。
// 繼承 /inherit/dummy：不還手、不閃避、不格擋、不損壞，偷襲必定成功。
#include <ansi.h>
inherit "/inherit/dummy";

void create()
{
    set_name("石製假人", ({ "stone stake", "stake", "stone" }));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("long", "一具用整塊青石鑿成的人形假人﹐底座深深埋進土裡﹐四肢與軀幹只粗略雕出輪廓﹐"
        "石面上滿是斑駁的拳印和刀劍砍鑿的痕跡。\n");
    setup();
}
