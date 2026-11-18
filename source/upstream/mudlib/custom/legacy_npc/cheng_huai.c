// /custom/legacy_npc/cheng_huai.c — 舊站 NPC：程槐（Cheng huai）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：未能判定；房間：—（2026-10-30 放入紫煙鎮茅屋）。
玄冰烈焰斧任務（2026-11-18，docs/martial_arts/玄冰烈焰斧_任務設計.md）：玩家交出血書與官印、並肯定是自己殺了縣令，
程槐證實全案，之後可向他求教玄冰烈焰斧（僅武者）。
尚未製作的裝備（先空著）：粗麻布衣(Cloth)。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("程槐", ({ "cheng huai", "cheng", "huai" }));
    set_race("human");
    set_class("fighter");
    set_level(25);
    set("gender", "male");
    set("age", 35);
    set("long",
        "程槐看起來非常強壯, 他雙手上長滿了因長期握斧而長的厚繭, 背上還背著一\n"
        "柄重斧, 據說他年輕時曾經以一把利斧「驂龍翔」挑了一個山寨的土匪, 讓道\n"
        "上盜匪都聞之喪膽, 不過看他現在臉色不太好, 好像是身上受了重傷。\n");
    set_skill("ice-fire axe", 100);   // 要會這門技能，玩家才能向他 acquire
    set_power("B");
    setup();
    carry_object("/custom/weapon/axe/wyvern_axe")->wield();   // 驂龍翔
}

// ------------------------------------------------------------------
// 玄冰烈焰斧任務。玩家身上的記號：
//   quest/ziyan_axe         0 未開始、1 查案中、2 案情大白、3 縣令伏誅（取得官印）
//   quest/ziyan_axe_letter  已把血書交給程槐
//   quest/ziyan_axe_seal    已把官印交給程槐
//   quest/ziyan_axe_done    解過任務（永久記號；之後可向程槐求教斧法）
// 任務可以一直重解（使用者指示，2026-11-18）：完成時把進度、線索、交付記號、密函記號全部清掉，
// 只留 done；再解一次又會得到贈禮（非武者或已學過者）。
// 每句話之間隔 1 tick（2 秒）；說話用 say，神情用 emote。
// ------------------------------------------------------------------

#define QUEST  "quest/ziyan_axe"
#define LETTER "quest/ziyan_axe_letter"
#define SEAL   "quest/ziyan_axe_seal"
#define DONE   "quest/ziyan_axe_done"
#define ASKING "ziyan_axe/asking"     // 程槐暫存：正在等誰回答「你殺了那個狗官」

private int still_here(object who)
{
    return objectp(who) && environment(who) == environment();
}

// 依序說話：lines 每項是一個指令字串；全部說完後呼叫 after（可為 0）。
void play(object who, string *lines, int i, string after)
{
    if( !living(this_object()) ) return;
    if( i >= sizeof(lines) ) {
        if( stringp(after) && still_here(who) ) call_other(this_object(), after, who);
        return;
    }
    command(lines[i]);
    call_out("play", 2, who, lines, i + 1, after);
}

private void talk(object who, string *lines, string after)
{
    call_out("play", 2, who, lines, 0, after);
}

// 走進茅屋：依進度有不同的神情；同一個人一分鐘內不重複。
void init()
{
    object who = this_player();
    string key;

    ::init();
    if( !objectp(who) || !userp(who) || !living(this_object()) ) return;
    key = "greet/" + who->query("id");
    if( query_temp(key) > time() - 60 ) return;
    set_temp(key, time());

    switch( (int)who->query(QUEST) ) {
    case 0: case 1:
        if( who->query(DONE) ) {
            if( who->query_class() == "fighter" && who->query_skill("ice-fire axe", 1) > 0 )
                talk(who, ({ "say 恩公又來了，斧法的訣竅，我已盡數相告，往後就看你自己的火候了。" }), 0);
            else
                talk(who, ({ "say 恩公又來了，那把斧子還使得順手嗎？" }), 0);
            break;
        }
        talk(who, ({ "emote 撐著身子坐直，一手悄悄按住背後的斧柄，冷冷地打量著你。",
                     "say 這裡沒有你要找的人，請回吧。" }), 0);
        break;
    default:
        if( who->query(LETTER) || who->query(SEAL) ) return;
        talk(who, ({ "emote 目光落在你身上，神色一動。",
                     "say 你是從魏家來的？",
                     "say 長青他……罷了，我身上有傷，進不了縣衙，沒有憑據，說什麼也沒人信。" }), 0);
    }
}

// 兩樣東西都交了：追問縣令是不是真的死了。
void ask_kill(object who)
{
    if( !still_here(who) ) return;
    command("say 你殺了那個狗官？");
    set_temp(ASKING, who);
    call_out("give_up", 60, who);   // 一直不回答就不等了，免得擋住別人
}

void give_up(object who)
{
    if( query_temp(ASKING) == who ) delete_temp(ASKING);
}

private void after_handover(object who)
{
    if( !who->query(SEAL) ) return;
    if( !who->query(LETTER) ) {
        talk(who, ({ "say 官印我認得，可是光有官印，長青的冤屈還說不明白，他留下的東西呢？" }), 0);
        return;
    }
    talk(who, ({ "emote 接過沉甸甸的官印，手指微微發抖，雙眼死死盯著印面。",
                 "say 這是趙弘春的縣令官印，從不離身。" }), "ask_kill");
}

void got_letter(object who)
{
    if( !still_here(who) ) return;
    if( who->query(SEAL) ) { after_handover(who); return; }
    talk(who, ({ "emote 雙手發顫地接過血書，指尖撫過那一道道血痕，眼眶漸漸紅了。",
                 "say 這是長青的字，他臨死還惦記著我。",
                 "say 光憑這封信，還扳不倒趙弘春，你若有膽，去取他那方從不離身的縣令官印來。" }), 0);
}

void got_seal(object who)
{
    if( !still_here(who) ) return;
    after_handover(who);
}

void got_secret(object who)
{
    if( !still_here(who) ) return;
    talk(who, ({ "emote 展開信紙細細讀過，臉色鐵青，握信的手背上青筋暴起。",
                 "say 原來他們早就勾搭上了，連分贓的日子都白紙黑字寫著。" }), 0);
}

void drop_item(object ob)
{
    if( objectp(ob) ) destruct(ob);
}

int accept_object(object who, object ob)
{
    string file = base_name(ob);

    if( (int)who->query(QUEST) < 2 ) {
        command("say 這是什麼東西？我不認得，你拿回去吧。");
        return 0;
    }
    if( query_temp(ASKING) ) return 0;   // 正在問話：先不收別的東西

    if( file == "/d/ziyan/obj/bloody_letter" ) {
        if( who->query(LETTER) ) return 0;
        who->set(LETTER, 1);
        call_out("got_letter", 1, who);
    } else if( file == "/d/ziyan/obj/magistrate_seal" ) {
        if( who->query(SEAL) || ob->query("quest_owner") != who->query("id") ) {
            command("say 這方官印不是你從趙弘春身上取來的吧？");
            return 0;
        }
        who->set(SEAL, 1);
        call_out("got_seal", 1, who);
    } else if( file == "/d/ziyan/obj/secret_letter" ) {
        if( ob->query("quest_owner") != who->query("id") ) return 0;
        call_out("got_secret", 1, who);
    } else
        return 0;

    call_out("drop_item", 1, ob);   // give 指令只有有價物品會銷毀；其他的由這裡收走
    return 1;
}

// 玩家回答：說「是」「對」「沒錯」「殺了」之類（不含「不是」「沒有」）都算肯定。
private int positive(string arg)
{
    if( !stringp(arg) ) return 0;
    if( strsrch(arg, "不") != -1 || strsrch(arg, "沒有") != -1 || strsrch(arg, "沒殺") != -1 ) return 0;
    foreach( string w in ({ "是", "對", "沒錯", "正是", "不錯", "殺了", "當然", "嗯", "恩", "yes", "Yes" }) )
        if( strsrch(arg, w) != -1 ) return 1;
    return 0;
}

private void answered(object who, int yes)
{
    delete_temp(ASKING);
    if( !yes ) {
        talk(who, ({ "emote 眉頭一皺，目光冷了下來。",
                     "say 這是大事，你想清楚了再回答我。" }), "ask_kill");
        return;
    }
    // 解完任務：進度與各項記號清掉（可以重解），只留永久記號 done。
    who->set(DONE, 1);
    who->set(QUEST, 0);
    who->delete("quest/ziyan_axe_clue");
    who->delete(LETTER);
    who->delete(SEAL);
    who->delete("quest/ziyan_axe_secret");
    // 學不了玄冰烈焰斧的人（不是武者，或已經學過）：道謝之後送一把闇雲雙刃斧答謝。
    if( who->query_class() != "fighter" || who->query_skill("ice-fire axe", 1) > 0 ) {
        talk(who, ({ "emote 重重地吐出一口長氣，眼角隱隱泛起淚光。",
                     "say 好！好！長青九泉之下也能瞑目了。",
                     "say 我程槐欠你一個天大的人情。",
                     "emote 從床板下摸出一把通身墨黑的雙刃斧，雙手遞了過來。",
                     "say 這是我早年得來的一把利刃，我如今握不動斧了，留著也是蒙塵。",
                     "say 無以為報，就以此斧答謝，請務必收下。" }), "give_reward");
        return;
    }
    talk(who, ({ "emote 重重地吐出一口長氣，眼角隱隱泛起淚光。",
                 "say 好！好！長青九泉之下也能瞑目了。",
                 "say 我程槐欠你一個天大的人情。",
                 "emote 解下背上的重斧，在掌心緩緩摩挲。",
                 "say 這把玄冰烈焰斧是我程家世代相傳的家學。",
                 "say 如今我身負重傷，再也握不動它，與其埋沒，不如傳給你這樣的人。" }), 0);
}

// 答謝的闇雲雙刃斧：每解完一次任務送一把。
void give_reward(object who)
{
    object axe;

    if( !still_here(who) ) return;
    axe = new("/custom/weapon/axe/blackcloud_axe");
    message_vision("$N把" + axe->name() + "交給$n。\n", this_object(), who);
    if( !axe->move(who) ) axe->move(environment());
}

void relay_say(object who, string arg)
{
    if( query_temp(ASKING) != who ) return;
    if( positive(arg) ) answered(who, 1);
    else if( strsrch(arg, "不") != -1 || strsrch(arg, "沒") != -1 ) answered(who, 0);
}

void relay_emote(object who, string verb)
{
    if( query_temp(ASKING) != who ) return;
    if( member_array(verb, ({ "nod", "smile", "grin", "bow", "agree", "ya", "sm" })) != -1 ) answered(who, 1);
    else if( verb == "shake" ) answered(who, 0);
}

// ------------------------------------------------------------------
// 傳授玄冰烈焰斧：任務完成才教，僅武者；補足到剛好學成。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( skill != "ice-fire axe" ) return 0;
    if( !me->query(DONE) ) {
        command("say 我跟你非親非故的，憑什麼教你家傳的斧法？");
        return 0;
    }
    if( me->query_class() != "fighter" ) {
        command("say 這路斧法是程家祖傳的功夫，需有武者的底子才練得來，你還是另謀高就吧。");
        return 0;
    }
    if( !SKILL_D("ice-fire axe")->valid_learn(me) ) return 0;
    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("程槐說道：訣竅都傳給你了，往後就看你自己的火候。\n");

    tell_object(me, CYN + "程槐將玄冰烈焰斧的運勁法門與招式要訣，一字一句地說給你聽。\n" NOR);
    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    return 1;
}
