// /d/ziyan/obj/magistrate_seal.c — 縣令官印：玄冰烈焰斧任務，擊殺趙弘春後從屍體裡取得。
// 記錄持有人（quest_owner），只有那位玩家交給程槐才算數。設計見 docs/martial_arts/玄冰烈焰斧_任務設計.md。

inherit ITEM;

void create()
{
    set_name("縣令官印", ({ "magistrate seal", "seal" }));
    set_weight(300);
    if( !clonep() ) {
        set("unit", "方");
        set("value", 0);
        set("long", "一方沉甸甸的銅鑄官印，印紐雕成一隻蹲伏的石獅，印面刻著酉縣縣令的篆文，\n"
                    "邊角被摩挲得發亮。\n");
    }
    setup();
}
