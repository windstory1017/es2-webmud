# 項鍊、鞋子、頭部裝備（custom/armor/neck、feet、head）

依使用者提供的 ES2 原始護具資料（Big5 文字檔）製作：項鍊 13 件、鞋子 27 件、頭部 28 件。
名稱、顏色碼、敘述與護具特性照原資料；所屬 NPC 與放置位置另行設定。

## 規則

- **穿戴位置**：項鍊（含護身符天鳴之符、混沌之鏡、靈骨）`neck_eq`；鞋子 `feet_eq`；
  頭部（含髮簪、髮釵、鬼面、飛涎羽）`head_eq`。各只能戴一件。
- **指令名稱**：英文全名或主詞（`X of Y` 取 X 的最後一字，否則取最後一字，例如 `necklace`、`rosary`、`boots`、`helmet`）。
  同名的英文照原資料保留（草鞋與軟皮靴都是 `boots`、黑布鞋與皂步靴都是 `black boots`），檔名分開。
- **名稱**：「蛈蜇杳龍屐」「啖紅裐雲靴」依使用者確認照原資料。
- **重量**（穿戴沒有膂力限制）：
  - 項鍊：珠串、金鍊 200；靈骨 300；混沌之鏡 500；鐵佛珠、金剛念珠、罡羅環 800。
  - 鞋子：草、布、絲、羽 500；皮、毛 1000；金屬 2000。
  - 頭部：髮簪、髮釵、羽毛 100；頭巾、布帽、斗笠、道冠 300；皮帽、骨盔、面具、青銅道冠 800；金屬頭盔 2000。
- **價值**：原資料有紀錄的照紀錄（黑斗笠原資料為五十文錢）。沒有紀錄的：
  - 項鍊：依重量，最高 300 兩（200 → 100～150、300 → 150～200、500 → 200～250、800 → 250～300 兩）。
  - 鞋子、頭部：比照衣服分級（普通 ≤50、中等 100～300、珍品依重量越重越貴，最高 1000 兩）。
- **唯一性**（`F_UNIQUE`）：虎牙項鏈、蛈蜇杳龍屐、六合玲瓏靴、獅吞獸面盔。
- **職業限制**：闇之項鏈、闇之寶靴限盜賊；霸王銅冠限軍人。
- **沿用既有物品**（數值與原資料相同，不另做）：皂步靴 `d/snow/npc/obj/clothboot.c`、
  古銅髮簪 `d/snow/npc/obj/hairpin.c`（雪亭鎮廢墟解謎）、精鋼戰靴 `custom/zhenwu/obj/steel_boots.c`。
- **只做物品**：天鳴之符（水月村解謎）、如是菩提（范老兒）、鬼面與虎首盔（冥靈子換物）、獸骨盔（吳天山）的
  取得劇情另行製作。靈骨同時是冥靈子換物的材料之一。

## 項鍊

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 如是菩提 | `neck/bodhi_rosary.c` | bodhi rosary | 200 | 150 | 依重量 |  |
| 黃銅佛珠 | `neck/copper_rosary.c` | copper rosary | 200 | 20 | 原資料 |  |
| 闇之項鏈 | `neck/dark_necklace.c` | dark necklace | 200 | 30 | 原資料 | 限盜賊 |
| 罡羅環 | `neck/dipper_necklace.c` | dipper necklace | 800 | 250 | 依重量 |  |
| 靈骨 | `neck/dragon_bone.c` | dragon bone | 300 | 20 | 原資料 |  |
| 鐵佛珠 | `neck/iron_rosary.c` | iron rosary | 800 | 300 | 依重量 |  |
| 混沌之鏡 | `neck/mirror_of_chaos.c` | mirror of chaos | 500 | 50 | 原資料 |  |
| 龍兒鎖 | `neck/necklace_of_daughter.c` | necklace of daughter | 200 | 150 | 依重量 |  |
| 金剛念珠 | `neck/psakaml.c` | psakaml | 800 | 250 | 依重量 |  |
| 紅寶石項鍊 | `neck/ruby_necklace.c` | ruby necklace | 200 | 100 | 依重量 |  |
| 暗默珠 | `neck/silance_rosary.c` | silance rosary | 200 | 100 | 原資料 |  |
| 天鳴之符 | `neck/tan_amulet.c` | tan amulet | 200 | 150 | 依重量 |  |
| 虎牙項鏈 | `neck/tiger_necklace.c` | tiger necklace | 200 | 150 | 依重量 | 唯一 |

## 鞋子

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 黑布鞋 | `feet/black_cloth_boots.c` | black boots | 500 | 10 | 普通 |  |
| 塵不沾 | `feet/boots_of_cleanse.c` | boots of cleanse | 500 | 750 | 原資料 |  |
| 奔雲靴 | `feet/boots_of_flying_cloud.c` | boots of flying cloud | 500 | 250 | 原資料 |  |
| 蠻牛靴 | `feet/bull_boots.c` | bull boots | 1000 | 80 | 原資料 |  |
| 短靴 | `feet/buskins.c` | buskins | 500 | 50 | 普通 |  |
| 流雲靴 | `feet/cloud_boots.c` | cloud boots | 1000 | 40 | 原資料 |  |
| 鱷皮靴 | `feet/crocodile_boots.c` | crocodile boots | 1000 | 30 | 原資料 |  |
| 闇之寶靴 | `feet/dark_boots.c` | dark boots | 500 | 50 | 原資料 | 限盜賊 |
| 蛈蜇杳龍屐 | `feet/dragon_boots.c` | dragon boots | 500 | 450 | 珍品 | 唯一 |
| 繡花鞋 | `feet/embroidery_boots.c` | embroidery boots | 500 | 30 | 原資料 |  |
| 小繡花鞋 | `feet/flower_boots.c` | flower boots | 500 | 20 | 原資料 |  |
| 流雲鐵靴 | `feet/flycloud_boots.c` | flycloud boots | 2000 | 35 | 原資料 |  |
| 飛天靴 | `feet/flying_boots.c` | flying boots | 1000 | 150 | 原資料 |  |
| 封神靴 | `feet/forgod_boots.c` | forgod boots | 2000 | 750 | 珍品 |  |
| 青雲戰靴 | `feet/green_boots_of_soldier.c` | green boots of soldier | 2000 | 250 | 中等 |  |
| 鐵丐靴 | `feet/iron_boots.c` | iron boots | 2000 | 300 | 中等 |  |
| 啖紅裐雲靴 | `feet/red_boots.c` | red boots | 500 | 100 | 中等 |  |
| 火鱗靴 | `feet/red_scale_boots.c` | red_scale boots | 1000 | 100 | 原資料 |  |
| 青蟒靴 | `feet/snakeskin_boots.c` | snakeskin boots | 1000 | 50 | 原資料 |  |
| 軟皮靴 | `feet/soft_leather_boots.c` | boots | 1000 | 5 | 原資料 |  |
| 玄蛛皂白靴 | `feet/spider_boots.c` | spider boots | 500 | 150 | 原資料 |  |
| 草鞋 | `feet/straw_boots.c` | boots | 500 | 1.2 | 原資料 |  |
| 紫紗靴 | `feet/violet_shoes.c` | violet shoes | 500 | 150 | 中等 |  |
| 狼皮靴 | `feet/wolf_boots.c` | wolf boots | 1000 | 250 | 中等 |  |
| 六合玲瓏靴 | `feet/wonder_boots.c` | wonder boots | 1000 | 700 | 珍品 | 唯一 |

## 頭部

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 價值來源 | 備註 |
|---|---|---|---|---|---|---|
| 步軍戰盔 | `head/battle_helm.c` | battle helm | 2000 | 200 | 中等 |  |
| 飛涎羽 | `head/bigbird_feather.c` | bigbird feather | 100 | 400 | 珍品 |  |
| 黑斗笠 | `head/black_doli.c` | black doli | 300 | 0.5 | 原資料 |  |
| 黃呢鐵帽 | `head/brass_hat.c` | brass hat | 2000 | 32 | 原資料 |  |
| 青銅道冠 | `head/copper_hat.c` | copper hat | 800 | 10 | 普通 |  |
| 飛鷹盔 | `head/eagle_helmet.c` | eagle helmet | 2000 | 250 | 中等 |  |
| 鬼面 | `head/ghost_mask.c` | ghost mask | 800 | 600 | 珍品 |  |
| 神羊帽 | `head/goat_hat.c` | hat | 800 | 150 | 中等 |  |
| 金羽神盔 | `head/golden_feather_helm.c` | helm | 2000 | 120 | 原資料 |  |
| 紫紗逍遙巾 | `head/headgear.c` | headgear | 300 | 50 | 原資料 |  |
| 鐵盔 | `head/iron_helmet.c` | iron helmet | 2000 | 15 | 普通 |  |
| 翠玉髮簪 | `head/jade_hairpin.c` | jade hairpin | 100 | 100 | 原資料 |  |
| 綠玉頂戴 | `head/jade_hat.c` | jade hat | 300 | 300 | 原資料 |  |
| 獅吞獸面盔 | `head/lion_helmet.c` | lion helmet | 2000 | 500 | 原資料 | 唯一 |
| 狙首盔 | `head/monkey_helmet.c` | monkey helmet | 800 | 50 | 原資料 |  |
| 紫雲盔 | `head/purple_helmet.c` | purple helmet | 2000 | 120 | 中等 | 2026-11-14 改為智慧 +1、精神 +1、防禦 25，重量與價值同金羽神盔（華子儀裝備） |
| 鮮紅道冠 | `head/red_hat.c` | red hat | 300 | 10 | 原資料 |  |
| 火鱗盔 | `head/red_scale_helm.c` | red_scale helm | 800 | 150 | 原資料 |  |
| 霸王銅冠 | `head/royal_helmet.c` | royal helmet | 2000 | 40 | 原資料 | 限軍人 |
| 獸骨盔 | `head/skull_helmet.c` | skull helmet | 800 | 500 | 原資料 |  |
| 遮陽大帽 | `head/sun_helmet.c` | sun helmet | 300 | 5 | 原資料 |  |
| 虎首盔 | `head/tiger_helmet.c` | tiger helmet | 2000 | 250 | 中等 |  |
| 紫玉釵 | `head/violet_hairpin.c` | violet hairpin | 100 | 250 | 中等 |  |
| 戰帽 | `head/war_hat.c` | war hat | 300 | 10 | 原資料 |  |
| 白馬盔 | `head/white_helmet.c` | white helmet | 2000 | 50 | 原資料 |  |
| 風獅吞面盔 | `head/wind_lion_helmet.c` | wind lion helmet | 800 | 300 | 中等 |  |
| 狼皮帽 | `head/woof_hat.c` | woof hat | 800 | 10 | 普通 |  |
