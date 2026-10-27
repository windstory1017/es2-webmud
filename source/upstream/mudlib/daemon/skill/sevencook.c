/*---
description: 廚燒七訣（NEW 新版武功寫法）。共通徒手武功，七招對應柴米油鹽醬醋茶，
             以灶台市井的日常動作出招；強度低於門派武功，沒有特技。
             設計見 docs/martial_arts/共通_廚燒七訣.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "sevencook";
    art_name  = "廚燒七訣";
    art_usage = "unarmed";
    art_desc  = "一套藏在灶台邊的拳掌功夫，七招對應柴米油鹽醬醋茶，"
                + "出手全是劈柴淘米、撒鹽沖茶的日常動作，看似平淡卻自有門道。";
    art_bonus = 0;

    moves = ({
        ([ "attack": 5, "defense": 3, "force": 1, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N右掌高高舉起﹐像在灶邊劈柴那樣隨手往下一落﹐這一記「柴字訣」直劈$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 0, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N使出「米字訣」﹐雙手一搓一抖好似淘米篩糠﹐指尖細細碎碎地點向$n的$l" ]),
        ([ "attack": 5, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N左掌滑溜溜地從來勢底下鑽過﹐正是「油字訣」﹐右掌像熱鍋潑油般燙向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N三根手指輕輕一撮﹐彷彿往鍋裡撒一把鹽﹐「鹽字訣」的指勁卻一粒粒彈在$n的$l" ]),
        ([ "attack": 4, "defense": 3, "force": 2, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N右拳慢吞吞地轉了半圈﹐倒像攪著一缸陳年老醬﹐「醬字訣」後勁沉沉地搗向$n的$l" ]),
        ([ "attack": 5, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N一招「醋字訣」左手往前一掀﹐像揭開醋罈子般﹐右手趁著$n一怔拍向$n的$l" ]),
        ([ "attack": 4, "defense": 5, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N氣定神閒地抬起右手﹐好似茶館夥計提壺沖茶﹐「茶字訣」由高而低連點$n的$l" ]),
    });

    /* 學成：累積到 5 級門檻時 gain，直接練成 5 級；學成那一級起每級給武術造詣 等級 × 10。 */
    art_entry        = 5;
    art_entry_reward = 1;
    art_entry_msg    = "你練成了廚燒七訣。\n";

    /* 每級武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("sevencook");
    setup();
}
