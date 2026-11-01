/* 風泉之劍 — 步玄派掌門駱易的家傳寶劍（使用者定案，2026-11-01）。
 *
 * - 唯一：世上只會有一把。被玩家取走後，駱易重生時改拿逐風刃（replica_ob）。
 * - 傷害：單手比照現有單手劍最高值（3, 20, 150, 5），雙手比照現有雙手劍最高值（4, 26, 180, 5）。
 * - 重量同萬夫莫敵（15000），價值 120000。
 * - 單手：攻勢等級 50、攻擊能力值 50、守勢等級 50、防禦能力值 50。
 *   雙手：膂力 3、傷害力 30、攻勢等級 50、攻擊能力值 50。
 * - 名稱亮青色；敘述依駱步玄與風神族夜鷹賭賽輕功的典故撰寫。
 * 設計見 docs/martial_arts/步玄派_門派設計.md。
 */
#include <ansi.h>
#include <weapon.h>

inherit F_UNIQUE;
inherit F_SWORD;

void create()
{
    set_name(HIC "風泉之劍" NOR, ({ "sword of wind spring", "wind spring", "sword" }));
    set_weight(15000);
    init_damage(3, 20, 150, 5, "sword");
    init_damage(4, 26, 180, 5, "twohanded sword");

    if( !clonep() ) {
        set("wield_as", ({ "sword", "twohanded sword" }));
        set("unit", "把");
        set("value", 120000);
        set("replica_ob", "/custom/weapon/sword/windfury_sword");
        set("long",
            "風泉之劍是步玄派掌門人代代相傳的佩劍，劍長五尺，琉璃護手，象牙劍\n"
            "柄，劍身明澈如鏡，隱隱透出一片淡青色的微光，彷彿有清風在劍脊上流\n"
            "動。\n"
            "    相傳步玄派始祖駱步玄七十歲那年，在練火大山的無名山谷中誤闖風神\n"
            "族的聚會。風神族的英雄「夜鷹」見他身法不凡，便以族中寶劍為注，與\n"
            "他賭賽輕功。兩人自谷底直上峰頂，駱步玄以一步之差勝出，夜鷹信守諾\n"
            "言，當場將此劍相贈。駱步玄晚年常說，「創步玄功」、「得風泉劍」、\n"
            "「娶吾賢妻」是他生平三大樂事。\n");
        set("apply_weapon/sword", ([
            "intimidate": 50,
            "attack": 50,
            "wittiness": 50,
            "defense": 50,
        ]));
        set("apply_weapon/twohanded sword", ([
            "str": 3,
            "damage": 30,
            "intimidate": 50,
            "attack": 50,
        ]));
    }
    setup();
}
