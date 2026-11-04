// /d/wutang/npc/yung_tai.c — 五堂鎮 NPC：雍泰（依五堂鎮設計表）。
//
// 風嘯拳法任務（使用者指示，2026-11-04，依舊資料 Quest 紀錄）：
// 玩家走進醇雨樓客棧時，雍泰向玩家打聽失散的四弟，每句間隔 1 tick（2 秒）。
// - 已學會風嘯拳法（fonxiaoken）的玩家不再觸發；
// - 一段話還沒說完時，其他人進來不會重頭再說；
// - 玩家中途離開，雍泰就不再往下說。

#include <ansi.h>
#include <npc.h>
inherit F_FIGHTER;

#define TICK 2                  // 一個 tick 的秒數

// 進房時的對話，$N 是雍泰，$n 是剛進來的玩家。
string *GREET_MSG = ({
    "$N望了$n幾眼﹐忽然說道﹕這位朋友可是外地來的﹖\n",
    "$N接著說道﹕小弟想打聽一個人﹐是我那失散的四弟 ...\n",
    "$N說道﹕我那四弟個頭比我高些﹐但是瘦了點﹐前年在斐縣境中失散 ...\n",
    "$N嘆了口氣﹐說道﹕到現在一點消息也沒有﹐也不知道是生是死 ...\n",
    "$N說道﹕如果您有見到或聽到舍弟的消息﹐請千萬讓小弟知道 ...\n",
    "$N嘆了口氣﹐又繼續喝酒。\n",
});

int talking;                    // 正在說進房的那段話

void create()
{
    seteuid(getuid());
    set_name("雍泰", ({ "yung tai", "yung", "tai" }));
    set_race("yenhold");
    set_class("fighter");
    set_level(20);
    set("age", 30);
    set("long", @LONG
雍泰是五堂鎮雍家這一代的老三﹐自從前年雍老爺從斐縣官期任滿
回鄉中途不幸被盜匪殺害﹐雍家四個兒子便流離各處﹐雍泰是幾個
月前才得到同鄉好友景襄仁景二爺幫助才得以湊足盤纏扶著老父棺
木回鄉安葬﹐雍家世代或為文官﹐或為武將﹐都是兩袖清風﹐家財
向來不富﹐但是卻相當受到五堂鎮居民的敬重﹐想不到這一代遭逢
厄運﹐眼看著就要沒落了。
LONG
    );
    set_skill("unarmed", 40);
    set_skill("dodge", 40);
    set_skill("parry", 40);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "雍泰獨自坐在角落喝著悶酒，望著窗外的鐵旗桿怔怔出神。\n",
        "雍泰嘆了口氣：「先父一生清廉，到頭來卻落得這般下場……」\n",
    }));
    set_power("B");
    setup();
}

void init()
{
    object who = this_player();

    if( talking || !userp(who) || !living(this_object()) || is_fighting() ) return;
    if( who->query_skill("fonxiaoken", 1) ) return;
    talking = 1;
    // 說話時先停下平常的閒聊，免得插在對話中間。
    set("chat_chance", 0);
    call_out("greet", TICK, who, 0);
}

void greet(object who, int step)
{
    if( !objectp(who) || environment(who) != environment()
    ||  !living(this_object()) || is_fighting() ) {
        talking = 0;
        set("chat_chance", 15);
        return;
    }
    message_vision(CYN + GREET_MSG[step] + NOR, this_object(), who);
    if( ++step < sizeof(GREET_MSG) ) {
        call_out("greet", TICK, who, step);
        return;
    }
    talking = 0;
    set("chat_chance", 15);
}
