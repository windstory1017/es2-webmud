// /d/ziyan/obj/secret_letter.c — 密函：山賊回覆趙弘春的信，玄冰烈焰斧任務的劇情道具（任務進度不需要）。
// 擊殺趙弘春時從屍體裡取得，記錄持有人（quest_owner）。設計見 docs/martial_arts/玄冰烈焰斧_任務設計.md。

inherit ITEM;

void create()
{
    set_name("密函", ({ "secret letter", "secret" }));
    set_weight(50);
    if( !clonep() ) {
        set("unit", "封");
        set("value", 0);
        set("long",
            "    趙大人台鑒：來信已悉。那批鏢銀共計若干，點數無誤，已照前議分作\n"
            "七三，三成留在山上弟兄喝酒，七成明日便著人送進縣衙後門。魏長青那廝\n"
            "押鏢有些門道，弟兄們折了幾人才拿下，大人既說要他一家老小不留活口，\n"
            "這等髒活，山上自有人替大人辦妥。往後景隆錢莊的鏢再過酉縣，咱們照舊\n"
            "放一條路給大人，只盼大人在縣衙裡替山上多遮掩幾分，莫讓官兵進山來。\n"
            "    山上人拜上。\n");
    }
    setup();
}
