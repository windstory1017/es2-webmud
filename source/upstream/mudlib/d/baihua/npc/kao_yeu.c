// /d/baihua/npc/kao_yeu.c — 百花村 NPC：「穿雲槍」高玉（依使用者設計表，2026-10-20）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("高玉", ({ "kao yeu", "kao", "yeu" }));
    set_race("human");
    set_class("fighter");
    set_level(35);
    set("gender", "male");
    set("age", 35);
    set("long", @LONG
高玉是江湖上小有名氣的槍法好手，一桿長槍使得出神入化，人稱「穿雲槍」。此刻他倚著岩壁坐在平台一角，衣襟上血跡斑斑，臉色蒼白，胸口纏著的布條還隱隱滲出血來，顯然受了不輕的傷，只是一雙眼睛依舊銳利，始終緊握著手中的長槍。
LONG
    );
    set("nickname", "穿雲槍");
    // 穹槍訣尚未有武功檔，只給名稱與等級、不做對應（同 NPC 門派武功規則）。
    set_skill("sky_pike", 105);
    set_power("B");
    setup();
    carry_object("/custom/weapon/pike/dragon_bone_pike")->wield();
    carry_object("/custom/armor/head/golden_feather_helm")->wear();
    carry_object("/custom/armor/armor/ringmail")->wear();
    carry_object("/custom/armor/feet/forgod_boots")->wear();
}
