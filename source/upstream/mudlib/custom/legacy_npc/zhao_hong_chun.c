// /custom/legacy_npc/zhao_hong_chun.c — 舊站 NPC：趙弘春（Zhao hong chun）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：紫煙鎮；房間：—。
尚未製作的裝備（先空著）：長袍馬掛(Robe)。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("趙弘春", ({ "zhao hong chun", "zhao", "chun" }));
    set("title", "縣令");
    set_race("human");
    set_class("commoner");
    set_level(5);
    set("gender", "male");
    set("age", 75);
    set("long",
        "趙弘春是酉縣的縣令, 不過謠傳說他的官位是花銀子買來的, 再加上\n"
        "他平時總是常常結交一些達官貴人, 因此在仕途上也是一帆風順, 不\n"
        "過紫煙鎮的居民卻普遍對這位縣令沒有好感, 據說雲鶴山的盜匪就是\n"
        "因為和他作對, 而被官府通緝, 被迫躲進雲鶴山據地為王, 也因此趙\n"
        "弘春處心積慮的想要消滅那群盜匪, 以除其心腹大患.\n");
    set_power("C");
    setup();
}

/* 玄冰烈焰斧任務（docs/martial_arts/玄冰烈焰斧_任務設計.md）：
 * 案情大白（quest/ziyan_axe 為 2）的玩家擊殺趙弘春，屍體裡多出一方官印；密函第一次另外掉一份。
 * 官印與密函都記錄持有人（quest_owner），官印交給程槐後才算完成，所以還沒交出官印之前可以重新取。 */
void die()
{
    object killer = last_damage_giver(), corpse, seal, letter;
    int stage;

    ::die();
    if( !objectp(killer) || !userp(killer) ) return;
    stage = (int)killer->query("quest/ziyan_axe");
    if( (stage != 2 && stage != 3) || killer->query("quest/ziyan_axe_seal") ) return;
    if( !objectp(corpse = query_temp("corpse")) ) return;

    seal = new("/d/ziyan/obj/magistrate_seal");
    seal->set("quest_owner", killer->query("id"));
    seal->move(corpse);
    if( !killer->query("quest/ziyan_axe_secret") ) {
        killer->set("quest/ziyan_axe_secret", 1);
        letter = new("/d/ziyan/obj/secret_letter");
        letter->set("quest_owner", killer->query("id"));
        letter->move(corpse);
    }
    killer->set("quest/ziyan_axe", 3);
    tell_object(killer, "你在趙弘春的屍身上摸到一方沉甸甸的官印，這是他從不離身的東西。\n");
}
