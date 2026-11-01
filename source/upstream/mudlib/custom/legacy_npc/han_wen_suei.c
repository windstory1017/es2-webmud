// /custom/legacy_npc/han_wen_suei.c — 舊站 NPC：韓文緒（Han wen-suei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：喬陰縣；房間：—。

互保（使用者指示，2026-11-01）：讓韓文緒接上官兵（F_SOLDIER）的互保系統，
與同房間的官兵雙向救援。不改繼承、保留武人母體原有的切磋反應，只補上官兵那套
「被動手時向房間求援」的 accept_kill，以及「收到求援就加入戰鬥」的 soldier_assist
兩個函式（行為與 /feature/npc/soldier.c 一致）。只有真的動手（kill）才互救，切磋不算。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("韓文緒", ({ "han wen-suei", "han", "wen-suei" }));
    set("title", "喬陰縣督護衛");
    set_race("yenhold");
    set_class("soldier");
    set_level(30);
    set("gender", "male");
    set("age", 35);
    set("long",
        "韓文緒是喬陰縣城的都護衛，負責喬陰縣的軍務工作，他的武藝雖然還\n"
        "稱不上一流高手，但是家傳的韓家槍法倒也已有八九分火候，在江湖上\n"
        "頗有些名頭。\n");
    set_power("B");
    setup();
    carry_object("/d/snow/npc/obj/clothboot")->wear();   // 皂步靴
    carry_object("/custom/armor/cloth/cyan_cloth")->wear();   // 青布官服
    carry_object("/custom/weapon/pike/thunder_pike")->wield();   // 紫電紅纓槍
    carry_object("/custom/armor/head/battle_helm")->wear();   // 步軍戰盔
    carry_object("/custom/armor/armor/battle_armor")->wear();   // 步軍戰鎧
    carry_object("/obj/cloth")->wear();   // 布衣
}

// 自己被動手時：向整個房間求援。身上有 soldier_assist 的（官兵、以及韓文緒自己）
// 才會回應，其餘人物／物品收到呼叫等於沒事。
void accept_kill(object ob)
{
    if( is_fighting(ob) ) return;
    do_chat(name() + "勃然變色，喝道﹕好大的膽子﹗來人哪﹐與我拿下這個狂徒﹗\n");
    all_inventory(environment())->soldier_assist(this_object(), ob);
}

// 收到同房間官兵的求援：只要自己還活著、又還沒在打這個人，就一同出手。
void soldier_assist(object alert, object ob)
{
    if( alert==this_object()
    ||    !living(this_object())
    ||    is_fighting(ob) )
        return;

    do_chat(name() + "沉喝一聲﹕光天化日之下也敢逞兇﹐待某來會你﹗\n");
    kill_ob(ob);
    ob->fight_ob(this_object());
}
