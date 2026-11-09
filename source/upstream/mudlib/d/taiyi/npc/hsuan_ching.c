// /d/taiyi/npc/hsuan_ching.c — 太乙觀：玄清道長（Hsuan-ching，依太乙觀設計表，2026-11-09）。
/*
白齊的得意門徒（使用者更正，2026-11-09），45 歲。使用者指示：玄清有謎題，會傳授太乙劍法，
任務之後再寫；目前只是站在山崖邊的道長。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("玄清道長", ({ "hsuan-ching", "taoist", "玄清道長" }));
    set_race("human");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 45);
    set("title", "太乙觀道長");
    set("long", @LONG
一位身著青衣道袍的道長，眉宇間帶著幾分落拓不羈，神情總像在思索著什麼。他是太乙觀觀主白齊的得意門徒，手中那柄桃木劍看似尋常，卻隱隱透著一股深不可測的氣度。
LONG
    );
    set("custom_faction", "taoist.freeze");   // 素衣派（docs/martial_arts/門派武功對照表.md）
    set("chat_chance_combat", 50);
    set("chat_msg_combat", ({
        (: command, "cast 1" :),
        (: command, "cast 2" :),
        (: command, "cast 3" :),
        (: command, "cast 4" :)
    }));
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
    carry_object("/custom/armor/cloth/green_robe")->wear();   // 青衣道袍
}
