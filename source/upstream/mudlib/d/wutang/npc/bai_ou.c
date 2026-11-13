/*---
description: 五堂鎮 NPC：白甌（鳳娘）。昔日宮裡的繡娘，替玩家把布衣繡出花樣。
design: docs/EMBROIDERY.md（使用者指示，2026-11-13）
rule: - 玩家對她說「繡花」開始；先給工錢（五兩黃金＝50000 文以上，多給不找，不足不收、不可分次），
        再給布衣（/obj/area/obj/cloth）；順序錯、給錯東西、給繡過的衣服都不收。
      - 收下布衣後原布衣銷毀（give 時店家收下有價值的物品會自動銷毀），3 tick 後產生新的繡花布衣
        （obj/area/obj/embroidered_cloth.c）交給玩家，繡花結果在那個檔案裡擲。
      - 玩家中途走開，繡好的衣服就沒了；白甌被殺，製作中斷，錢與布衣都不補。
      - 每位玩家各自進行（進度記在白甌身上，不記在玩家身上）。
---*/

#include <npc.h>
inherit F_VILLAGER;

#define FEE         50000                       // 五兩黃金（文）
#define TICK        2                           // 一個 tick 的秒數
#define EMBROIDER_TICKS 3                       // 繡一件要幾個 tick
#define CLOTH_FILE  "/obj/area/obj/cloth"
#define EMBROIDERED_FILE "/obj/area/obj/embroidered_cloth"

mapping paid = ([]);                            // 已付工錢、等著交布衣的玩家

void create()
{
    seteuid(getuid());
    set_name("白甌", ({ "bai ou", "bai", "ou" }));
    set("nickname", "鳳娘");
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "female");
    set("age", 38);
    set("long", @LONG
一位身著月白繡裙的婦人，指尖拈著一縷細如蠶絲的絲線，眉間靜如止水。傳聞她昔年是宮裡最得寵的繡娘，龍袍鳳帔皆出其手，如今卻不知為何歸於五堂鎮，在這間布莊裡執針度日。蘇繡之細、湘繡之神、粵繡之華、蜀繡之嚴，到了她指下渾然一體，早已別開一派。
LONG
    );
    set_skill("unarmed", 10);
    set_skill("dodge", 10);
    set_skill("parry", 10);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "白甌執針穿線，素指翻飛，繡繃上的花樣漸漸有了形狀。\n",
        "白甌將絲線湊近唇邊輕輕一抿，劈作更細的幾縷。\n",
        "白甌輕聲自語：「尋常的布衣，若肯繡花，也能別有一番風致。」\n",
        "白甌抬眼望向遠處，不知想起了什麼，半晌才又低下頭去，繼續手上的針線。\n",
    }));
    set_power("C");
    setup();
}

/* ---- 說話與給東西 ------------------------------------------------- */

void invite(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    if( paid[who] ) {
        message_vision("$N手中的針線不停，淡淡地說道：「工錢我已收下了，把布衣拿來吧 (give cloth to bai ou)。」\n",
            this_object());
        return;
    }
    message_vision("$N擱下手中的繡繃，抬眸淡淡一笑：「繡花麼？也罷，難得有人識貨。取五兩黃金作工錢"
        " (give 5 gold to bai ou)，再備一件素淨的布衣 (give cloth to bai ou)，我替你繡上幾針。"
        "記著，先付工錢，後交衣裳，這是我的規矩。」\n", this_object());
}

void relay_say(object who, string msg)
{
    if( !userp(who) || !stringp(msg) || strsrch(msg, "繡花") == -1 ) return;
    call_out("invite", 1, who);
}

void say_to(string msg)
{
    message_vision("$N" + msg + "\n", this_object());
}

// 給東西：傳回 1 才算收下（收下有價值的東西，give 會把它銷毀）。
int accept_object(object who, object ob)
{
    if( !userp(who) ) return 0;

    if( ob->query("money_id") ) {
        if( paid[who] ) {
            say_to("搖了搖頭：「工錢你已經付過了，不必再給。把布衣拿來吧 (give cloth to bai ou)。」");
            return 0;
        }
        if( ob->value() < FEE ) {
            say_to("瞥了一眼掌中的錢，輕輕搖頭：「我這一針一線，可不是尋常價錢。五兩黃金，少一文也不成。」");
            return 0;
        }
        paid[who] = 1;
        call_out("say_to", 0, "將金子收入袖中，唇邊浮起一抹淺笑：「這才像話。衣裳呢？取來讓我瞧瞧 (give cloth to bai ou)。」");
        return 1;
    }

    if( ob->query("embroidered") ) {
        say_to("撫過衣上的針腳，微微蹙眉：「這件已有人下過針了，我不願在別人的針腳上再添一筆。」");
        return 0;
    }
    if( base_name(ob) != CLOTH_FILE ) {
        say_to("淡淡掃了一眼：「我的針只為布衣 (cloth) 而動，這東西，恕我不繡。」");
        return 0;
    }
    if( !paid[who] ) {
        say_to("眼也不抬，指尖輕輕推開衣裳：「針線未動，先談酬勞，這是規矩。先把五兩黃金交給我"
            " (give 5 gold to bai ou)，再來說衣裳的事。」");
        return 0;
    }

    map_delete(paid, who);
    call_out("begin", 0, who);
    return 1;
}

/* ---- 繡花 ---------------------------------------------------------- */

void begin(object who)
{
    message_vision("$N接過$n遞來的布衣，在膝上輕輕撫平，信手劈開一縷絲線，細如蠶吐。"
        "素指翻飛，銀針在布面上起落如蝶，轉眼已見雲紋隱隱。\n", this_object(), who);
    call_out("finish", EMBROIDER_TICKS * TICK, who);
}

void finish(object who)
{
    object ob;

    // 玩家走開（或離線）就沒了，不補。
    if( !objectp(who) || environment(who) != environment() ) {
        message_vision("$N見人已走遠，輕嘆一聲，將繡好的衣裳收了起來。\n", this_object());
        return;
    }

    seteuid(getuid());
    ob = new(EMBROIDERED_FILE);
    ob->embroider();
    if( !ob->move(who) ) ob->move(environment());

    message_vision("$N咬斷線頭，將衣裳輕輕一抖，雲霞般的針腳在布上流轉。她把衣裳遞給$n：「成了。」\n",
        this_object(), who);
    call_out("farewell", TICK, who, ob->query("embroidery_result"));
}

// 繡花結果對應的一句話（結果代號見 embroidered_cloth.c）。
private string result_line(int result)
{
    switch(result) {
    case 1: return "針腳裡藏了幾分靈氣，穿著它，人也精神些。";
    case 2: return "這一路針法帶著鋒芒，出手時自有幾分銳氣。";
    case 3: return "今日心緒有些浮，針腳走偏了鋒芒，莫怪於我。";
    case 4: return "密針織成屏障，尋常刀兵難近。";
    case 5: return "針腳鬆了幾分，守勢怕是弱了些，莫怪。";
    case 6: return "這一路針法難得，是我少有的得意之作。";
    case 7: return "針腳輕靈，穿上它，身形當更見靈動。";
    case 8: return "我在襟邊暗藏了一線機巧，風吹草動，你自會察覺。";
    }
    return "";
}

void farewell(object who, int result)
{
    message_vision("$N望著那件衣裳，目光柔了幾分：「一針一線，都是我的心血。往後穿著它行走江湖，"
        "風吹雨打也好，刀光劍影也罷，只盼你好好待它，莫讓它蒙了塵，更別隨手棄於道旁。"
        "待它如待故人，它自會護你周全。」\n", this_object());
    call_out("farewell2", TICK, who, result);
}

void farewell2(object who, int result)
{
    message_vision("$N" + (stringp(result_line(result)) && result_line(result) != "" ? "又低聲補了一句：「"
        + result_line(result) + "」" : "微微頷首") + "\n", this_object());
    if( objectp(who) && environment(who) == environment() )
        message_vision("$N輕聲道：「它還沒有名字，你替它取一個吧 (name cloth as 名稱)，想改幾次都行。」\n",
            this_object());
}
