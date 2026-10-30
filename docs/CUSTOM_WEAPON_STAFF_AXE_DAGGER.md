# 杖、斧、匕首（custom/weapon/staff、axe、dagger）

依使用者提供的 ES2 原始武器資料（Big5 文字檔）製作：杖 21 筆、斧 16 筆、匕首 23 筆。
名稱、顏色碼、敘述、傷害與武器特性照原資料；所屬 NPC 與放置位置另行設定。規則比照刀類（`docs/CUSTOM_WEAPON_BLADE.md`）。

## 規則

- **指令名稱**：英文全名或 `staff`／`axe`／`dagger`。
- **傷害**：以 `init_damage()` 直接還原原資料，`identify` 顯示的數字與原資料相同。
- **重量**：有單手用法的依單手最高傷害換算（26 → 膂力 8、84 → 24，最少 5）；只有雙手用法的依雙手最高傷害
  （36 → 12、124 → 30）。十石大斧依原資料「需求膂力 31」設為 32000；開光有 invoke 的雨神之牙依原資料「膂力 35」設為 36000。
  （膂力比需求少 1 仍可勉強使用，規則見 `std/race/humanoid.c` 的 `valid_wield()`。）
- **價值**：原資料有紀錄的照紀錄；沒有的在 50～1000 兩之間取 50 的倍數。
- **特性套用的用法**：
  - 匕首「錐法、左手錐法」同一段列出的，特性兩手都有（椎心刺、血刃原資料註明「左右手一樣」）。
  - 只寫在其中一種用法後面的，只給那一種：「赤」只有左手錐法、黑鋼杖只有雙握杖法、青龍白虎斧只有左手斧術、
    闇雲雙刃斧只有雙手斧術。
  - 谷玉的特性只給杖法；「法力值 20」即魔力附加 `magic_ability`（`identify` 顯示「魔力」）。
  - 焚之魔杖第二組傷害與特性為雙握杖法。
- **唯一性**：冥魔杖、血月斧、血匕封喉、「雨毒」、萬夫莫敵。
- **「雨毒」的劇毒**：見 `docs/CUSTOM_WEAPON_WHIP_NEEDLE_BLUNT_PIKE_SWORD.md`（`rain_poison`）。
- **冥魔杖特攻**：攻擊被閃躲、格擋，或命中但力道被完全吸收時 10% 機率發動，敘述「冥魔杖上的蛇頭突然睜開雙眼，一股血腥之氣直撲而去！」，
  不經任何防禦直接扣對方的氣：(10 + (膽識 + 膂力) / 3) x (1 + 10% x 殺人數)，殺人數用 PK 紀錄 `pk_record`。
  戰鬥系統在武器攻擊沒造成傷害（閃躲、格擋、力道被完全吸收）後呼叫武器的 `miss_ob(攻擊者, 目標)`（`adm/daemons/combatd.c`），之後其他武器也可以使用。
- **雨神之牙三個版本**：未開光（`rainlords_tooth`，價值 800 兩）、開光有 invoke（`rainlords_tooth_invoke`，需要膂力 35，
  限哭笑門待門派製作後補上）、開光後（`rainlords_tooth_awakened`）。insert／pull、freeze pill 與 invoke 依使用者指示不製作。
- **雪魂匕**：兩個版本合併為一把，不設唯一：傷害 10-52，根骨 2、膽識 1、冰寒傷害防禦力 30、攻擊能力值 5。
- **沿用既有物品**：「尺娘」沿用瞎眼老太婆的 `d/snow/npc/obj/syndicator.c`（傷害相同）；萬夫莫敵沿用真武區米沛的
  `custom/zhenwu/obj/great_axe_of_mighty.c`，依原資料改為價值 2000 兩並加上唯一性。

## 杖

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 黑鋼杖 | `staff/black_iron_staff.c` | black-iron staff | 2600 | 950 | staff、twohanded staff | 價值隨機 |
| 白骨杖 | `staff/bone_staff.c` | bone staff | 2600 | 1000 | staff、twohanded staff | 價值隨機 |
| 移殤杖 | `staff/change_cane.c` | change cane | 14000 | 250 | twohanded staff | 價值隨機 |
| 古銅杖 | `staff/copper_staff.c` | copper staff | 14000 | 350 | twohanded staff | 價值隨機 |
| 龍頭柺 | `staff/dragon_head_cane.c` | dragon-head cane | 13200 | 150 | twohanded staff |  |
| 焚之魔杖 | `staff/flame_staff.c` | flame staff | 2500 | 300 | staff、twohanded staff | 價值隨機 |
| 九曲十彎 | `staff/god_cane.c` | god cane | 21400 | 400 | twohanded staff | 價值隨機 |
| 金龍杖 | `staff/golden_dragon_cane.c` | golden dragon cane | 17300 | 150 | twohanded staff | 價值隨機 |
| 符紙咒傘 | `staff/incantation_umbrella.c` | incantation umbrella | 12000 | 100 | twohanded staff | 價值隨機 |
| 木杖 | `staff/long_staff.c` | long staff | 12000 | 550 | twohanded staff | 價值隨機 |
| 雨神之牙 | `staff/rainlords_tooth.c` | rainlord's tooth | 21400 | 800 | twohanded staff |  |
| 雨神之牙 | `staff/rainlords_tooth_awakened.c` | rainlord's tooth | 21400 | 650 | twohanded staff | 價值隨機 |
| 雨神之牙 | `staff/rainlords_tooth_invoke.c` | rainlord's tooth | 36000 | 750 | twohanded staff | 價值隨機 |
| 禁符之杖 | `staff/rune_staff.c` | rune staff | 10200 | 210 | twohanded staff |  |
| 冥魔杖 | `staff/serpent_cane.c` | serpent cane | 19000 | 850 | twohanded staff | 唯一、價值隨機 |
| 萬年雪 | `staff/staff_of_ancient_snow.c` | staff of ancient snow | 11200 | 65 | twohanded staff |  |
| 交歡杖 | `staff/staff_of_eroticism.c` | staff of eroticism | 17300 | 200 | twohanded staff | 價值隨機 |
| 谷玉 | `staff/staff_of_wisdom.c` | staff of wisdom | 2600 | 250 | staff、twohanded staff | 價值隨機 |
| 玄烏杖 | `staff/strange_copper_staff.c` | stange-copper staff | 17300 | 650 | twohanded staff | 價值隨機 |
| 枯禪杖 | `staff/wither_zenstaff.c` | wither zenstaff | 19000 | 700 | twohanded staff |  |
| 禪杖 | `staff/zenstaff.c` | zenstaff | 15700 | 600 | twohanded staff | 價值隨機 |

## 斧

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 血月斧 | `axe/axe_of_bloodmoon.c` | axe of bloodmoon | 6900 | 350 | axe、secondhand axe | 唯一、價值隨機 |
| 闇雲雙刃斧 | `axe/blackcloud_axe.c` | blackcloud axe | 6800 | 50 | twohanded axe、axe | 價值隨機 |
| 嗜血巨斧 | `axe/blood_axe.c` | blood axe | 16500 | 400 | twohanded axe | 價值隨機 |
| 斬龍斧 | `axe/dragon_killer_axe.c` | dragon-killer axe | 24900 | 550 | twohanded axe | 價值隨機 |
| 青龍白虎斧 | `axe/dragon_tiger_axe.c` | dragon-tiger axe | 10000 | 300 | axe、secondhand axe | 數值照水嵐縣設計表（2026-10-30，敘述照原資料）：單手、左手都是膂力 +1、傷害力 10；密林大統領 |
| 八鬼牙斧 | `axe/eight_ghost_axe.c` | eight-ghost axe | 24900 | 900 | twohanded axe | 價值隨機 |
| 萬夫巨斧 | `axe/ghost_axe.c` | ghost axe | 20000 | 3 | twohanded axe |  |
| 十石大斧 | `axe/grandaxe.c` | grandaxe | 32000 | 200 | twohanded axe |  |
| 地煞斧 | `axe/hell_twibil.c` | hell twibil | 20000 | 350 | twohanded axe | 價值隨機 |
| 破魔斧 | `axe/holy_axe.c` | holy axe | 23200 | 400 | twohanded axe | 價值隨機 |
| 巨斧 | `axe/huge_axe.c` | huge axe | 23200 | 950 | twohanded axe | 價值隨機 |
| 厚柄斧 | `axe/large_axe.c` | large axe | 6900 | 650 | axe | 價值隨機 |
| 深朱闊斧 | `axe/red_axe.c` | red axe | 15500 | 550 | twohanded axe | 價值隨機 |
| 血月短斧 | `axe/short_axe_of_bloodmoon.c` | short axe of bloodmoon | 7900 | 350 | axe、secondhand axe | 價值隨機 |
| 驂龍翔 | `axe/wyvern_axe.c` | wyvern axe | 16500 | 450 | twohanded axe | 價值隨機 |

## 匕首

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 七彩琉璃匕 | `dagger/beauty_dagger.c` | beauty dagger | 6300 | 50 | dagger、secondhand dagger |  |
| 黑鋼短刃 | `dagger/black_iron_dagger.c` | black iron dagger | 5700 | 40 | dagger、secondhand dagger |  |
| 血匕封喉 | `dagger/bloody_dagger.c` | bloody dagger | 7600 | 750 | dagger、secondhand dagger | 唯一、價值隨機 |
| 蝴蝶短刺 | `dagger/butterfly_stiletto.c` | butterfly stiletto | 4400 | 650 | dagger、secondhand dagger | 價值隨機 |
| 麗光短刃 | `dagger/charming_dagger.c` | charming dagger | 6300 | 73 | dagger、secondhand dagger |  |
| 「赤」 | `dagger/chi_dagger.c` | chi dagger | 5700 | 450 | dagger、secondhand dagger | 價值隨機 |
| 幽雲短匕 | `dagger/cloud_dagger.c` | cloud dagger | 7600 | 40 | dagger、secondhand dagger |  |
| 寒鐵匕首 | `dagger/cool_iron_dagger.c` | cool-iron dagger | 5700 | 50 | dagger、secondhand dagger |  |
| 血刃 | `dagger/cruel_dagger.c` | cruel dagger | 6300 | 850 | dagger、secondhand dagger | 價值隨機 |
| 椎心刺 | `dagger/dire_spike.c` | dire spike | 7600 | 100 | dagger、secondhand dagger |  |
| 暴龍牙 | `dagger/dragon_dagger.c` | dragon dagger | 5700 | 1 | dagger、secondhand dagger |  |
| 罪牙匕 | `dagger/evil_grin_dagger.c` | evil-grin dagger | 8800 | 150 | dagger、secondhand dagger |  |
| 赤龍牙骨 | `dagger/fire_dragon_tooth.c` | fire-dragon tooth | 6300 | 50 | dagger、secondhand dagger |  |
| 梅花匕 | `dagger/lunmay_dagger.c` | lunmay dagger | 5700 | 650 | dagger、secondhand dagger | 價值隨機 |
| 穿心刺 | `dagger/piercing_dagger.c` | dagger | 5700 | 20 | dagger、secondhand dagger |  |
| 影匕 | `dagger/shadow_dagger.c` | shadow dagger | 4400 | 100 | dagger、secondhand dagger | 價值隨機 |
| 神農匕 | `dagger/shennong_dagger.c` | dagger | 2500 | 750 | dagger、secondhand dagger | 價值隨機 |
| 雪魂匕 | `dagger/snow_dagger.c` | snow dagger | 7600 | 600 | dagger、secondhand dagger | 價值隨機 |
| 「雨毒」 | `dagger/wicked_dagger.c` | wicked dagger | 7600 | 300 | dagger、secondhand dagger | 唯一、價值隨機 |
| 狼匕 | `dagger/wolf_dagger.c` | wolf dagger | 4400 | 35 | dagger、secondhand dagger |  |
| 「魈」 | `dagger/xiao_dagger.c` | xiao dagger | 5700 | 800 | secondhand dagger、dagger | 價值隨機 |
