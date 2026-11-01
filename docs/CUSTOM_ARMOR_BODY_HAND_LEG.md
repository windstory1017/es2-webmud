# 護甲、手部、腿部裝備（custom/armor/armor、hand、leg）

依使用者提供的 ES2 原始護具資料（Big5 文字檔）製作：護甲 40 件、手部 22 件、腿部 7 件。
名稱、顏色碼、敘述與護具特性照原資料；所屬 NPC 與放置位置另行設定。

## 規則

- **穿戴位置**：護甲（含肩甲、心鏡）`armor`，可與衣服同時穿；手套、護腕、爪 `hand_eq`；腿護、護膝、綁腿 `leg_eq`。
- **指令名稱**：英文全名或主詞；同名英文照原資料保留（Dragon armor 有 4 件、Steel armor 與 Iron armor 各 2 件），檔名分開。
- **重量**（穿戴沒有膂力限制）：
  - 護甲：絲甲、軟甲、藤甲 3000；皮甲、鱗甲 5000；鎖子甲、鏈甲 8000；板甲、鎧 12000；肩甲、肩鎧、心鏡 2000。
  - 手部：布、絲 200；皮 400；鐵、爪 800。腿部：布、竹 300；皮 600；金屬、鱗 1000。
- **價值**：原資料有紀錄的照紀錄。沒有紀錄的：
  - 護甲：普通 ≤100、中等 150～500、珍品 500～1500 兩（依重量：2000 → 500～700、3000 → 600～800、
    5000 → 800～1000、8000 → 1000～1250、12000 → 1250～1500）。
  - 手部、腿部：比照衣服分級。
- **唯一性**：火鷹戰鎧。
- **職業限制**：霸王鎧、霸王手套（原註 sr only）限軍人；闇之絲甲、闇之護手限盜賊；靈中樞甲（原資料「天師」）限道士。
- **玄晦戰甲**：原資料「可以讀內功」。繼承 `F_STUDY`，用 `study black harness` 研讀，基本內功（`force`）最高讀到 100，
  規則與書本相同（耗神、依悟性增加進度）。
- **新技能代碼**（登記於 `docs/SKILL_NAMES.md`）：暗器技巧 `throwing`（混天白龍）、天邪神掌 `celestial palm`（天邪虎爪）。
- **改依原資料的既有物品**（米沛身上）：`custom/zhenwu/obj/steel_armor.c` 精鋼戰甲補上傷害力 5；
  `custom/zhenwu/obj/fire_gauntlets.c` 冶磺套改為防禦力 5、火焰傷害防禦力 15、雙手斧術 10，價值 200 兩。重量維持原值。
- **絕版的聖龍甲也製作**。

## 護甲

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 紫羅點翠戰甲 | `armor/armor_of_violet_rohan.c` | armor of violet rohan | 8000 | 50 | 原資料 |  |
| 藤甲 | `armor/banded_armor.c` | banded armor | 3000 | 20 | 原資料 |  |
| 白熊護甲 | `armor/bear_armor.c` | bear armor | 5000 | 350 | 中等 |  |
| 玄晦戰甲 | `armor/black_harness.c` | black harness | 5000 | 2500 | 原資料 | 可研讀內功 |
| 赤龍鱗 | `armor/brimstony_armor.c` | brimstony armor | 5000 | 250 | 原資料 |  |
| 鎖鏈背心 | `armor/chain_shirt.c` | chain shirt | 8000 | 300 | 原資料 |  |
| 闇之絲甲 | `armor/dark_armor.c` | dark armor | 3000 | 100 | 原資料 | 限盜賊 |
| 兩儀雲龍甲 | `armor/double_cloudy_plate.c` | double cloudy plate | 12000 | 200 | 原資料 |  |
| 怒龍錦冑 | `armor/dragon_armor.c` | dragon armor | 5000 | 1000 | 原資料 |  |
| 龍鱗璧玉袍 | `armor/dragon_jade_robe.c` | dragon armor | 5000 | 200 | 原資料（敘述）；數值照水嵐縣設計表 | 防禦力 20、防禦能力值 10、火焰防禦 10、冰防禦 20（2026-10-30）；密林賊帥 |
| 龍騎戰甲 | `armor/dragon_plate_of_knights.c` | dragon plate of knights | 12000 | 300 | 原資料 |  |
| 火鷹戰鎧 | `armor/full_plate_of_fire_hawk.c` | full plate of fire hawk | 12000 | 1000 | 原資料 | 唯一 |
| 冥魂戰鎧 | `armor/ghost_hell_plate.c` | ghost-hell plate | 12000 | 300 | 原資料 |  |
| 冥龍甲 | `armor/hell_dragon_armor.c` | hell dragon armor | 12000 | 300 | 原資料 |  |
| 聖龍甲 | `armor/holy_dragon_armor.c` | dragon armor | 8000 | 1200 | 珍品 |  |
| 鐵戰甲 | `armor/iron_armor.c` | iron armor | 12000 | 500 | 原資料 |  |
| 鑌鐵胸鎧 | `armor/iron_torso.c` | iron torso | 12000 | 150 | 原資料 |  |
| 光鎧 | `armor/light_plate.c` | light plate | 12000 | 33 | 原資料 |  |
| 法胄 | `armor/magic_armor.c` | magic armor | 3000 | 250 | 原資料 |  |
| 猿仙絲冑 | `armor/monkey_armor.c` | monkey armor | 3000 | 800 | 原資料 |  |
| 軟櫬甲 | `armor/padded_armor.c` | padded armor | 3000 | 95 | 原資料 |  |
| 紫雲戰甲 | `armor/purple_cloud_mail.c` | purple-cloud mail | 12000 | 100 | 原資料 |  |
| 生鐵戰甲 | `armor/raw_iron_armor.c` | iron armor | 12000 | 450 | 中等 |  |
| 火鱗胸鎧 | `armor/red_scale_plate.c` | red_scale plate | 5000 | 500 | 原資料 |  |
| 連身鎖子甲 | `armor/ring_armor.c` | ring armor | 8000 | 50 | 普通 |  |
| 鎖子甲 | `armor/ringmail.c` | ringmail | 8000 | 80 | 原資料 |  |
| 霸王鎧 | `armor/royal_armor.c` | royal armor | 12000 | 150 | 原資料 | 限軍人 |
| 鐵披甲 | `armor/scale_armor.c` | scale armor | 8000 | 90 | 普通 |  |
| 無形心鏡 | `armor/shapeless_heartguard.c` | shapeless heartguard | 2000 | 250 | 原資料 |  |
| 藍鋼肩甲 | `armor/shoulder_armor.c` | shoulder armor | 2000 | 95 | 原資料 |  |
| 肩鎧 | `armor/side_armor.c` | side armor | 2000 | 10 | 普通 |  |
| 銀霜鎧 | `armor/silver_platemail_of_frost.c` | silver platemail of frost | 12000 | 2500 | 原資料 |  |
| 天羅雲翳 | `armor/sky_cloud_mail.c` | sky cloud mail | 3000 | 600 | 珍品 |  |
| 靈中樞甲 | `armor/soul_lapis_lazuli_plate.c` | soul lapis lazuli plate | 12000 | 1400 | 珍品 | 限道士 |
| 鋼戰甲 | `armor/steel_chain_armor.c` | steel armor | 8000 | 450 | 中等 |  |
| 鑲釘皮鎧 | `armor/studded_armor.c` | studded armor | 5000 | 300 | 中等 |  |
| 虎嘯旭日鎧 | `armor/tiger_plate.c` | tiger plate | 12000 | 250 | 原資料 |  |
| 混天白龍 | `armor/white_dragon_plate.c` | white-dragon plate | 12000 | 1000 | 原資料 |  |
| 素虹軟甲 | `armor/white_rainbow_armor.c` | white rainbow armor | 3000 | 750 | 珍品 |  |

## 手部

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 白熊套手 | `hand/bear_gloves.c` | bear gloves | 400 | 200 | 中等 |  |
| 黑鐵護手 | `hand/black_gauntlets.c` | black gauntlets | 800 | 250 | 中等 |  |
| 黑手套 | `hand/black_gloves.c` | black gloves | 200 | 150 | 中等 |  |
| 暗金護腕 | `hand/bracers_of_darkness.c` | bracers of darkness | 200 | 200 | 中等 |  |
| 雍和爪 | `hand/brutal_beasts_claw.c` | brutal beast's claw | 800 | 250 | 中等 |  |
| 天邪虎爪 | `hand/celestial_tigers_claw.c` | celestial tiger's claw | 800 | 20 | 原資料 |  |
| 鱷皮手套 | `hand/crocodile_gloves.c` | crocodile gloves | 400 | 30 | 原資料 |  |
| 闇之護手 | `hand/dark_bracers.c` | dark bracers | 200 | 50 | 原資料 | 限盜賊 |
| 針織手套 | `hand/embroidery_gloves.c` | embroidery gloves | 200 | 50 | 普通 |  |
| 心殘爪 | `hand/heartless_claw.c` | heartless claw | 400 | 250 | 中等 |  |
| 玉蟒鱗護 | `hand/python_gloves.c` | python gloves | 400 | 100 | 中等 |  |
| 火鱗手套 | `hand/red_scale_gloves.c` | red_scale gloves | 400 | 100 | 原資料 |  |
| 霸王手套 | `hand/royal_gloves.c` | royal gloves | 400 | 100 | 中等 | 限軍人 |
| 蠶絲手套 | `hand/silk_gloves.c` | silk gloves | 200 | 10 | 原資料 |  |
| 鹿皮手套 | `hand/skin_gloves.c` | skin gloves | 400 | 10 | 普通 |  |
| 纏蛇護手 | `hand/snake_bracers.c` | snake bracers | 400 | 45 | 原資料 |  |
| 冷雪手護 | `hand/snowy_bracers.c` | snowy bracers | 200 | 100 | 中等 |  |
| 精鋼護腕 | `hand/steel_bracers.c` | steel bracers | 800 | 20 | 原資料 |  |
| 雷熊爪 | `hand/thunder_claw.c` | thunder claw | 800 | 50 | 原資料 |  |
| 狂雷手套 | `hand/thunder_gloves.c` | thunder gloves | 400 | 100 | 中等 |  |
| 疾風套手 | `hand/wind_gloves.c` | wind gloves | 400 | 30 | 原資料 |  |

## 腿部

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 青竹腿護 | `leg/bamboo_legs.c` | bamboo legs | 300 | 5 | 原資料 |  |
| 白熊護腿 | `leg/bear_legs.c` | bear legs | 600 | 10 | 普通 |  |
| 牛皮綁腿 | `leg/leather_leggings.c` | leather leggings | 600 | 5 | 原資料 |  |
| 風塵腿護 | `leg/red_dust_legs.c` | red-dust legs | 300 | 40 | 普通 |  |
| 火鱗脛甲 | `leg/red_scale_leggings.c` | red_scale leggings | 1000 | 100 | 原資料 |  |
| 星雲腿護 | `leg/star_studded_legs.c` | star_studded legs | 1000 | 50 | 原資料 |  |
| 烈陽護膝 | `leg/sun_legs.c` | sun legs | 600 | 50 | 原資料 |  |
