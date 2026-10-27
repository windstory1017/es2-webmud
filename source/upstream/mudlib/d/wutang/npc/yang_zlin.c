// /d/wutang/npc/yang_zlin.c — 五堂鎮 NPC：冷梅莊一代弟子 楊子陵（依五堂鎮設計表）。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("楊子陵", ({ "yang zlin", "yang", "zlin" }));
    set_race("human");
    set_class("fighter");
    set_level(45);
    set("title", "冷梅莊一代弟子");
    set("age", 30);
    set("long", @LONG
楊子陵在冷梅派第一代弟子中排行第四, 為梅影風某日出外於路上所遇到的棄嬰, 因機緣而入冷梅派, 目前劍術內功已略有小成, 一手真冷梅劍法在武林中也頗有微名。
LONG
    );
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "楊子陵負手而立，望著遠處的羿水，似乎在等什麼人。\n",
    }));
    set("custom_faction", "fighter.lunmay");   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/black_sword")->wield();
    carry_object("/custom/weapon/dagger/black_iron_dagger")->wield();
    carry_object("/custom/armor/cloth/silk_cloth")->wear();
    carry_object("/custom/armor/feet/wolf_boots")->wear();
    carry_object("/custom/armor/head/woof_hat")->wear();
}
