/*---
description: 五堂鎮 NPC：白甌（鳳娘）。昔日宮裡的繡娘，替玩家把布衣繡出花樣。
design: docs/EMBROIDERY.md（使用者指示，2026-11-13）
rule: - 玩家對她說「繡花」開始；先給工錢（五兩黃金＝50000 文以上，多給不找，不足不收、不可分次），
        再給布衣（/obj/area/obj/cloth）；順序錯、給錯東西、給繡過的衣服都不收。
      - 收下布衣後原布衣銷毀（give 時店家收下有價值的物品會自動銷毀），1 tick 後產生新的繡花布衣
        （obj/area/obj/embroidered_cloth.c）交給玩家，繡花結果在那個檔案裡擲。
      - 玩家中途走開，繡好的衣服就沒了；白甌被殺，製作中斷，錢與布衣都不補。
      - 每位玩家各自進行（進度記在白甌身上，不記在玩家身上）。
---*/

#include <ansi.h>
#include <npc.h>
inherit F_VILLAGER;

#define FEE         50000                       // 五兩黃金（文）
#define TICK        2                           // 一個 tick 的秒數
#define EMBROIDER_TICKS 1                       // 收下布衣後幾個 tick 繡好
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
// 白甌說的話照 say 的格式：「白甌說道﹕」加綠色內容（使用者指示，2026-11-13）；
// 動作的描述用表情的青色。

// 白甌說話：verb 是完整的說話動詞，例如「說道」「笑道」「接著說道」。
void talk(string verb, string text)
{
    message_vision("$N" + verb + "﹕" + HIG + text + NOR + "\n", this_object());
}

// pre 是「說道」前面的動作（可空），例如 "笑著" → 「白甌笑著說道﹕…」。
void speak(string pre, string text)
{
    talk(pre + "說道", text);
}

// 動作描述（青色，同表情）。
void act(string msg, object who)
{
    message_vision(CYN + msg + NOR + "\n", this_object(), who);
}

void invite(object who)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    if( paid[who] ) {
        speak("", "工錢我已收下了，把布衣拿來吧 (give cloth to bai ou)。");
        return;
    }
    act("$N擱下手中的繡繃，抬眸淡淡一笑。", who);
    // 一行放不下，分成幾句說（遊戲每行 70 格，綠色碼也佔格數）。
    speak("", "繡花嗎？難得有人識貨。");
    speak("接著", "只要工錢五兩黃金，我就能為你的布衣繡上幾針；");
    talk("笑道", "記得先付工錢、後交衣裳，這是繡坊一直以來的規矩。");
}

void relay_say(object who, string msg)
{
    if( !userp(who) || !stringp(msg) || strsrch(msg, "繡花") == -1 ) return;
    call_out("invite", 1, who);
}

// 給東西：傳回 1 才算收下（收下有價值的東西，give 會把它銷毀）。
int accept_object(object who, object ob)
{
    if( !userp(who) ) return 0;

    if( ob->query("money_id") ) {
        if( paid[who] ) {
            speak("", "工錢已經付過了，是哪件布衣想刺繡呢？");
            return 0;
        }
        if( ob->value() < FEE ) {
            talk("笑道", "五兩黃金，少一文也不成。");
            return 0;
        }
        paid[who] = 1;
        call_out("speak", 0, "笑著", "衣裳呢？取來讓我瞧瞧");
        return 1;
    }

    if( ob->query("embroidered") ) {
        speak("蹙眉", "這件已有人下過針了，我不願在別人的針腳上再添一筆。");
        return 0;
    }
    if( base_name(ob) != CLOTH_FILE ) {
        speak("", "我的針只為布衣 (cloth) 而動，這東西，恕我不繡。");
        return 0;
    }
    if( !paid[who] ) {
        // 還沒付工錢就遞衣裳：不說話，只是微笑（衣裳退回）。
        act("$N愉快地對著$n微笑著。", who);
        return 0;
    }

    map_delete(paid, who);
    call_out("begin", 0, who, ob);      // 等 give 把衣裳交到手上，再銷毀它
    return 1;
}

/* ---- 繡花 ---------------------------------------------------------- */

// 收下的布衣直接銷毀，不留在白甌身上（使用者指示，2026-11-14）。
// 布衣的 value() 查不到價錢，give 不會自動銷毀它，所以這裡自己銷毀。
void begin(object who, object cloth)
{
    if( objectp(cloth) ) destruct(cloth);
    act("$N接過$n遞來的布衣，銀針在布面上起落如蝶，轉眼已見雲紋隱隱。", who);
    call_out("finish", EMBROIDER_TICKS * TICK, who);
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

void finish(object who)
{
    object ob;

    // 玩家走開（或離線）就沒了，不補。
    if( !objectp(who) || environment(who) != environment() ) {
        message_vision(CYN "$N見人已走遠，輕嘆一聲，將繡好的衣裳收了起來。\n" NOR, this_object());
        return;
    }

    seteuid(getuid());
    ob = new(EMBROIDERED_FILE);
    ob->embroider();
    speak("", "成了，" + result_line(ob->query("embroidery_result")));

    // 這時候就把衣服交給玩家；拿不動就放在地上。
    if( !ob->move(who) ) ob->move(environment());
    message_vision("$N交給$n一" + ob->query("unit") + ob->name() + "。\n", this_object(), who);
}
