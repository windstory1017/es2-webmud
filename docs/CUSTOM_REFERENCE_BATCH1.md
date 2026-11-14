# 參考資料第一批：武器 26、護具 29

來源：`docs/REFERENCE_SCAN_UNREGISTERED.md` 的 A、B 兩節（es2tips 等參考資料，有完整數值的部分）。

## 規則

- **重量、價值**：與之前各批相同（見 `CUSTOM_WEAPON_*.md`、`CUSTOM_ARMOR_*.md`）。武器重量依最高傷害換算；價值沒有紀錄的取一次固定。軍備商、劉可的黃金售價不採用。
- **黑鐵大槌**：最高傷害 178，依同一換算為 41000（雙手需要膂力 41，可差 1）。
- **行動力**：用遊戲既有的 `move` 能力值（`query_ability("move")`、`apply/move`），`identify` 顯示「行動力」。
- **火焰／冰寒／雷電／風擊傷害力**：代碼 `damage_vs_fire`、`damage_vs_ice`、`damage_vs_lightning`、`damage_vs_wind`（沿用天師道法原本讀取的代碼），`identify` 顯示中文。
  - **本身就是元素傷害的**：厭火 breathe（火）、天師道法火術／冰咒／風符／雷法，在抗性判定**前**直接加上同元素的傷害力：
    咒文傷害 =（咒文威力 ＋ 傷害力）× 100 ÷（100 ＋ 目標同元素防禦力）。原本未被使用的「百分比加成」改為這個加法。
  - **其他技能**：招式標明元素（`action["element"]` 為 `fire`、`ice`、`lightning`、`wind`）時，命中並造成傷害後，
    另外加上裝備的同元素傷害力，再以目標同元素防禦力減免（`adm/daemons/combatd.c` 的 `elemental_extra()`）。
    元素不相符的傷害力不加成。目前還沒有招式標明元素。
- **玉戒尺**：特性「大邪心經」，代碼 `devil force`（2026-10-14 使用者確認大邪心經是另一套內功，原本當作大邪心法 `huge force`）。
- **蝶舞**：原文特性被截斷，依使用者指定為縱躍閃躲之法 +20、機敏 +2，刀法與左手刀法都有。
- **烏金皮**：穿在衣服位置。
- **種族裝備**：只有該種族能穿戴——焦僥靴（焦僥）、釘靈腿護（釘靈）、厭火之拳（厭火）、巫首項鍊（巫首）、無腸寶珠（無腸）。
- **黑鐵大槌**、**龍麟袋**、自訂英文名稱：使用者確認維持。
- **纏布刀／玉戒尺**：平常叫纏布刀（`wrapped blade`）；裝備時「你從纏布抽出一把亮晃晃的玉刀，緊握在手上。」，名稱變成玉戒尺（`green blade`）；解除時「你將手上的玉刀小心翼翼地收回纏布中。」，改回纏布刀。
- **饕餮法杖 knock**：帶在身上即可（不必裝備）。`knock <裝備或武器>`：物品必須先卸下，每 25 文價值換一張空白符紙，物品消失。不到 25 文的不收。
- **腐蝕之手 corrupt**：戴著才能用。`corrupt <屍體>`：屍體化成 10 ～（慧根＋定力）× 3 兩碎銀，放在地上；屍體裡的碎銀一併合在一起，地上原本有碎銀也疊上去；其他遺物掉在原地。玩家的屍體不行（`adm/daemons/chard.c` 為玩家屍體加上 `player_corpse` 記號）。
- **沒有英文名稱的**（自訂）：旱魃簪 `hanba hairpin`、舊銀髮簪 `old silver hairpin`、濁魚珠 `zhuoyu ring`、玉蛛佩 `jade spider girdle`。
- **其他特殊指令**（離玄光熾的金咒、天命刃 wish、歸心似劍 vow）先不做。

## 武器

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 備註 |
|---|---|---|---|---|---|
| 追月流星劍 | `sword/charm_sword.c` | charm sword | 8700 | 400 | sword, secondhand sword |
| 纏布刀／玉戒尺 | `blade/green_blade.c` | wrapped blade | 6100 | 550 | blade；平常為纏布刀，裝備時變成玉戒尺 |
| 鑲玉短劍 | `sword/jeweled_shortsword.c` | jeweled shortsword | 3300 | 600 | sword, secondhand sword |
| 雛鐵劍 | `sword/ironsword.c` | ironsword | 3300 | 850 | sword |
| 歸心似劍 | `sword/loving_sword.c` | loving sword | 3300 | 950 | sword, secondhand sword |
| 疾風劍 | `sword/rapid_sword.c` | rapid sword | 7900 | 650 | sword, secondhand sword |
| 西山仙劍 | `sword/godness_sword.c` | godness sword | 8700 | 200 | sword |
| 英雄王者劍 | `sword/royal_sword_of_honor.c` | royal sword of honor | 26900 | 1000 | twohanded sword |
| 古岳風刃 | `sword/flurry_sword.c` | flurry sword | 26900 | 250 | twohanded sword |
| 風雷刀 | `blade/thunderfurry_blade.c` | thunderfurry blade | 9200 | 350 | blade |
| 金絲八卦刀 | `blade/golden_blade.c` | golden blade | 9200 | 150 | blade |
| 戰神劈天刀 | `blade/warlord_blade.c` | warlord blade | 30000 | 250 | twohanded blade |
| 蝶舞 | `blade/butterfly_kris.c` | butterfly kris | 4000 | 100 | blade, secondhand blade |
| 古鄔匕 | `dagger/ancient_dagger.c` | ancient dagger | 8800 | 300 | dagger |
| 無情匕 | `dagger/coldblood_dagger.c` | coldblood dagger | 8800 | 750 | dagger |
| 天命刃 | `dagger/dagger_of_fate.c` | dagger of fate | 6300 | 650 | dagger |
| 「旋芒」 | `dagger/dimensional_dagger.c` | dimensional dagger | 7600 | 350 | dagger, secondhand dagger |
| 被腐蝕的旋芒 | `dagger/digested_holy_sword.c` | digested holy sword | 5700 | 550 | dagger, secondhand dagger |
| 繡花銀針 | `needle/tailors_needle.c` | tailor's needle | 500 | 350 | needle |
| 蓮花禪杖 | `staff/flower_cane.c` | flower cane | 21400 | 950 | twohanded staff |
| 聖靈禪杖 | `staff/cane_of_spirit.c` | cane of spirit | 19000 | 450 | twohanded staff |
| 裁決之杖 | `staff/staff_of_judgement.c` | staff of judgement | 19000 | 450 | twohanded staff |
| 饕餮法杖 | `staff/celestial_bull_cane.c` | celestial bull cane | 17700 | 650 | twohanded staff；knock 換空白符紙 |
| 離玄光熾 | `staff/black_spark.c` | black spark | 21400 | 350 | twohanded staff |
| 金龍豹紋槍 | `pike/golden_beast_lance.c` | golden beast lance | 23000 | 550 | twohanded pike |
| 黑鐵大槌 | `blunt/black_iron_hammer.c` | black iron hammer | 41000 | 50 | twohanded blunt |

## 補充：追風劍任務的兩把武器

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 備註 |
|---|---|---|---|---|---|
| 追風劍 | `sword/storm_chasers_sword.c` | storm chaser's sword | 8700 | 50 | sword；冰寒傷害力 50、傷害力 10、攻擊能力值 50 |
| 青絲釣竿 | `whip/silk_fishing_stick.c` | fishing stick | 6300 | 1000 | whip；捕魚技巧 10。英文與生鐵釣竿相同，照原資料保留。crash（把河魚碾成魚餌）等遊戲有河魚、魚餌後再做 |

## 護具

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 備註 |
|---|---|---|---|---|---|
| 青布官服 | `cloth/cyan_cloth.c` | cyan cloth | 1000 | 170 |  |
| 繭綢長袍 | `cloth/umber_robe.c` | umber robe | 1000 | 45 |  |
| 舊長袍 | `cloth/worn_robe.c` | worn robe | 2000 | 25 |  |
| 烏金皮 | `cloth/suncat_skin.c` | suncat skin | 4000 | 180 |  |
| 鎖子鎧 | `armor/ring_mail.c` | ring mail | 8000 | 80 |  |
| 狼皮護甲 | `armor/wolf_armor.c` | wolf armor | 5000 | 430 |  |
| 步軍戰鎧 | `armor/battle_armor.c` | battle armor | 12000 | 260 |  |
| 破蠡虎皮 | `armor/tiger_armor.c` | tiger armor | 5000 | 910 |  |
| 玉蟒頭巾 | `head/python_headgear.c` | python headgear | 300 | 220 |  |
| 珊瑚髮釵 | `head/coral_hairpin.c` | coral hairpin | 100 | 260 |  |
| 旱魃簪 | `head/hanba_hairpin.c` | hanba hairpin | 100 | 310 |  |
| 舊銀髮簪 | `head/old_silver_hairpin.c` | old silver hairpin | 100 | 38 |  |
| 巫首項鍊 | `neck/malik_necklace.c` | malik necklace | 200 | 130 |  |
| 赤魈心 | `neck/chixiao_necklace.c` | chixiao necklace | 200 | 140 |  |
| 白骨念珠 | `neck/skull_rosary.c` | skull rosary | 800 | 300 |  |
| 無腸寶珠 | `finger/woochan_ring.c` | woochan ring | 100 | 1310 |  |
| 濁魚珠 | `finger/zhuoyu_ring.c` | zhuoyu ring | 100 | 930 |  |
| 黃玉佩 | `waist/topaz_belt.c` | topaz belt | 800 | 16 |  |
| 金絲翡翠帶 | `waist/golden_jade_belt.c` | golden jade belt | 800 | 710 |  |
| 玉蛛佩 | `waist/jade_spider_girdle.c` | jade spider girdle | 800 | 650 |  |
| 厭火之拳 | `hand/yenhold_gauntlets.c` | yenhold gauntlets | 400 | 220 |  |
| 腐蝕之手 | `hand/corrosive_hands.c` | corrosive hands | 400 | 150 | corrupt 屍體換碎銀 |
| 青絲手套 | `hand/silky_gloves.c` | silky gloves | 200 | 230 |  |
| 英雄護手 | `hand/gloves_of_heroism.c` | gloves of heroism | 800 | 150 |  |
| 焦僥靴 | `feet/jiaojao_boots.c` | jiaojao's boots | 500 | 300 |  |
| 疾步快靴 | `feet/runners_boots.c` | runner's boots | 1000 | 760 |  |
| 英雄戰靴 | `feet/boots_of_heroism.c` | boots of heroism | 1000 | 190 |  |
| 釘靈腿護 | `leg/dingling_legs.c` | dingling legs | 1000 | 110 |  |
| 英雄護腿 | `leg/legs_of_heroism.c` | legs of heroism | 1000 | 250 |  |

## 符紙與容器

| 名稱 | 檔案 | 英文 | 重量 | 價值 | 敘述 |
|---|---|---|---|---|---|
| 空白符紙 | `custom/item/scroll/blank.c` | blank scroll | 每張 1 | 每張 10 文 | 一張空白的符紙。 |
| 正陽符 | `custom/item/scroll/yang.c` | scroll of yang | 每張 1 | 每張 50 文 | 一張隱隱透著浩然正氣的符紙。 |
| 少陰符 | `custom/item/scroll/yin.c` | scroll of yin | 每張 1 | 每張 50 文 | 一張冷冽透著陰氣的符紙。 |
| 先天符 | `custom/item/scroll/nature.c` | scroll of nature | 每張 1 | 每張 50 文 | 一張散發著自然的氣息的符紙。 |
| 龍麟袋 | `custom/item/dragon_skin_bag.c` | dragon-skin bag | 1000 | 20 兩黃金（2026-11-14 改，原 100 兩白銀） | 這個大袋子根本就是一張完整的龍麟﹐不怪竟能裝下這麼多東西。容量為麻布袋的 100 倍。 |

符紙可以疊在一起，顯示為「X張空白符紙」，四種都可用 `scroll` 指定。
