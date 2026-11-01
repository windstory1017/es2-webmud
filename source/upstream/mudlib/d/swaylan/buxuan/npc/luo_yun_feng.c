// /d/swaylan/buxuan/npc/luo_yun_feng.c — 落雲峰 NPC：駱雲峰（駱老三，依水嵐縣設計表）。
// 可向他求教共通技能雲峰筆招（daemon/skill/uenfon.c，設計見 docs/martial_arts/ 的雲峰筆招設計文件）。

#include <npc.h>
#include <ansi.h>
inherit F_SCHOLAR;

void create()
{
    seteuid(getuid());
    set_name("駱雲峰", ({ "luo yun-feng", "luo", "yun-feng" }));
    set_race("human");
    set_class("scholar");
    set_level(40);
    set("gender", "male");
    set("age", 30);
    set("long", @LONG
人稱駱老三，一身白色長袍，手中握著一支判官筆，舉手投足間帶著一股書卷氣。他在落雲峰上獨居多年，從山川雲氣中悟出雲峰筆招、雲體風身術與虛空斬三套絕技。
LONG
    );
    set_skill("unarmed", 80);
    set_skill("dodge", 80);
    set_skill("parry", 80);
    set_skill("literate", 120);
    set_skill("dagger", 80);
    set_skill("secondhand dagger", 80);
    set_skill("uenfon", 80);
    map_skill("dagger", "uenfon");
    map_skill("secondhand dagger", "uenfon");
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/white_scholar_dress")->wear();
    carry_object("/obj/area/obj/pen")->wield();
    carry_object("/obj/area/obj/pen")->wield("secondhand dagger");
}

// 傳授雲峰筆招：讀書識字夠的人才教，補足到剛好可以升到 1 級的經驗（同梅影風的傳授）。
int acquire_skill(object me, string skill)
{
    if( skill != "uenfon" ) return 0;
    if( !SKILL_D("uenfon")->valid_learn(me) ) return 0;
    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("駱雲峰笑道：訣竅都說給你聽了，往後就看你自己的體會。\n");

    message_vision("$N愉快地微笑著。\n", this_object());
    message_vision("$N說道：看在你對書法也有興趣的份上，就教了你吧。\n", this_object());
    tell_object(me, "駱雲峰對你說明「雲峰筆招」的訣竅。\n");
    tell_object(me, CYN "你聽了駱雲峰的指導後，對「雲峰筆招」有了一些體會。\n" NOR);
    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    return 1;
}
