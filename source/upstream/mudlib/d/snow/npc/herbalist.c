// herbalist.c

#include <npc.h>

inherit F_VILLAGER;
inherit "/obj/alchemist/pill_vendor";   // 二階丹藥三倍價（使用者指示 #6，2026-10-23）

void create()
{
	set_name("藥鋪掌櫃", ({"herbalist"}));
	set_race("human");
	set_class("commoner");
	set_level(3);
	set_skill("unarmed", 5);
	set_skill("parry", 5);
	set_skill("dodge", 5);
	set_skill("dagger", 5);

	set("age", 44);
	set("long",
		"藥鋪掌櫃的是位胖胖的中年男子﹐雖然年紀有四五十歲了﹐不過看起\n"
		"來依然紅光滿面、精神煥發﹐你可以用 list 問他店裡有哪些藥。\n");

	set("merchandise", ([
		"/obj/alchemist/black_pill" : 50,
		STOCK_MEDICATION("boar_berry") : 40,
		STOCK_MEDICATION("wild_ginseng") : 30,
		STOCK_MEDICATION("dragon_saliva") : 20,
		"/obj/alchemist/minor_heal_pill" : 10,		   // 小還丹
		"/obj/alchemist/mana_pill" : 10,				 // 醒神丹
		"/obj/alchemist/aquapill" : 50,				  // 金液丹
		"/obj/alchemist/refresh_pill" : 10,		      // 金鎖固精丸
		"/obj/alchemist/lavage_pill" : 5,              // 清腹丹
		"/obj/alchemist/relax_pill" : 5,               // 養心丹
		// 以上丹藥數量依使用者指示 #5（2026-10-23）
	]));

	set_power("C");
	setup();
	carry_object("/obj/area/obj/cloth")->wear();
	carry_object(__DIR__"obj/fur_hat")->wear();
}

void init()
{
	::init();
	add_action("do_vendor_list", "list");
}
