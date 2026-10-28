# 鞭、針、錘棒、槍、劍（custom/weapon/whip、needle、blunt、pike、sword）

依使用者提供的 ES2 原始武器資料（Big5 文字檔）製作：鞭 10、針 7、錘棒 7、槍 19、劍 56（原資料劍類中的冰雪匕放在匕首類）。
名稱、顏色碼、敘述、傷害與武器特性照原資料；所屬 NPC 與放置位置另行設定。規則比照 `docs/CUSTOM_WEAPON_STAFF_AXE_DAGGER.md`。

## 規則

- **重量**：同刀、杖、斧、匕首的換算；**針一律重 500，膂力 1 即可使用**（左手需要 2，可差 1）。
- **價值**：原資料有紀錄的照紀錄；沒有的在 50～1000 兩之間取 50 的倍數。
- **特性套用**：同一段列出兩種技能的，特性兩手都有；只寫在其中一種用法後面的，只給那一種（例如蜂尾針只有針術）。
- **原資料的技能名稱修正**：黑色短劍第二段為左手劍術；天都劍主手（劍術 7-49）、左手（7-25）都可用，特性只在左手。
- **唯一性**：毒龍鞭、附骨之蛆、射日長槍、憤怒明王槍、蔑天劍、蠻龍、儀陽劍、黃雀寶劍、虛靈劍、風泉之劍、古劍醒塵。
- **兩個版本**：「穿靈」（未合劍，`celestial_sword`）與『穿靈』（四劍合一，`celestial_sword_awakened`）照原資料；
  兩把不同的易羅大劍（`broadsword`、`yiluo_sword`）。
- **洗銀劍**：另做一把依原資料、亮白色的 `sword/silver_sword.c`；雪亭鎮方士身上的 `d/snow/npc/obj/silversword.c` 不變。
- **劇毒**（萬骨枯心、「雨毒」、百鬒寒鳩劍、憤怒明王槍、紫金鳳頭錐、附骨之蛆；共用規則 `custom/condition/weapon_poison.c`）：命中並造成傷害時上毒；再次命中只把剩餘次數重設為滿；
  每次發作「你中的毒發作了！」。表中「A/B」依使用者指定：A 為每次扣的**目前值**、B 為每次扣的**實格**（可隨時間恢復的上限，不是永久扣除）。
  每次發作先扣目前值、再扣實格，例如 500/500、每次 10/8：490/492、480/484、470/476。
  （藍涎刀的毒另依當時指定，見 `docs/CUSTOM_WEAPON_BLADE.md`。）

| 武器 | 毒（daemon/condition） | 中毒敘述 | 發作 | 每次 精 | 每次 氣 | 每次 神 |
|---|---|---|---|---|---|---|
| 萬骨枯心 | `skull_heart_poison` | 你的傷口已經麻木了，沒有任何反應！ | 2 tick × 4 次 | 5/3 | 8/5 | 2/1 |
| 「雨毒」（匕首） | `rain_poison` | 你覺得傷口處一陣麻癢，腦中一陣昏眩，顯然中了劇毒！ | 2 tick × 4 次 | 8/4 | 8/4 | 2/1 |
| 百鬒寒鳩劍 | `hundred_poison` | 你感到萬毒啐骨, 五臟六腑血氣翻騰！ | 3 tick × 3 次 | 7/3 | 10/5 | 2/1 |
| 憤怒明王槍 | `wraith_poison` | 你感到傷口一陣灼熱，恐是中毒了。 | 2 tick × 5 次 | 4/2 | 5/3 | 0/0 |
| 紫金鳳頭錐 | `phoenix_poison` | 你的傷口一陣痠麻，一股黑血從傷口湧出 | 2 tick × 5 次 | 4/2 | 5/3 | 0/0 |
| 附骨之蛆（針） | `maggot_poison` | 你感到一陣噁心，嘔出一灘帶血的膿痰，裡面竟然有幾隻蛆蟲蠕蠕而動！ | 2 tick × 5 次 | 10/8 | 8/5 | 4/3 |

## 鞭

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 怒天鞭 | `whip/angry_whip.c` | angry whip | 7300 | 50 | whip |  |
| 赤龍筋 | `whip/brimstony_whip.c` | brimstony whip | 6300 | 50 | whip |  |
| 毒龍鞭 | `whip/dragon_whip.c` | dragon whip | 6300 | 300 | whip | 唯一 |
| 生鐵釣竿 | `whip/fishing_stick.c` | fishing stick | 6300 | 75 | whip |  |
| 天王鞭 | `whip/god_whip.c` | god whip | 7300 | 900 | whip | 價值隨機 |
| 金絲拂塵 | `whip/golden_buddha_duster.c` | golden buddha duster | 6300 | 100 | whip |  |
| 辟雷鞭 | `whip/thunderproof_whip.c` | whip | 7300 | 150 | whip |  |
| 戰鞭 | `whip/war_whip.c` | war whip | 4800 | 600 | whip | 價值隨機 |
| 神農打穀鞭 | `whip/whip_of_alchemy_cereal.c` | whip of alchemy cereal | 4800 | 250 | whip | 價值隨機 |
| 六靈妖姬鞭 | `whip/whip_of_evil_beauties.c` | whip of evil beauties | 4800 | 300 | whip | 價值隨機 |

## 針

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 蜂尾針 | `needle/bee_needle.c` | bee needle | 500 | 650 | needle、secondhand needle | 價值隨機 |
| 盤龍針 | `needle/dragon_needle.c` | dragon needle | 500 | 150 | needle、secondhand needle | 價值隨機 |
| 繡花針 | `needle/embroidery_needle.c` | embroidery needle | 500 | 550 | needle、secondhand needle | 價值隨機 |
| 透骨釘 | `needle/iron_needle.c` | iron needle | 500 | 600 | needle、secondhand needle | 價值隨機 |
| 附骨之蛆 | `needle/needle_of_bone_maggot.c` | needle of bone maggot | 500 | 1000 | needle | 唯一、劇毒 |
| 銀針 | `needle/silver_needle.c` | silver needle | 500 | 2 | needle、secondhand needle | 售價 200 文（使用者指定，2026-10-28） |
| 鋼針 | `needle/steel_needle.c` | steel needle | 500 | 3 | needle、secondhand needle |  |

## 錘棒

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 碎腦鎚 | `blunt/crasher_maul.c` | crasher maul | 22400 | 800 | twohanded blunt | 價值隨機 |
| 寒玉藥杵 | `blunt/jade_rod.c` | jade rod | 6600 | 1000 | blunt | 價值隨機 |
| 萬骨枯心 | `blunt/skull_heart.c` | skull heart | 21800 | 200 | twohanded blunt | 劇毒 |
| 大金槌 | `blunt/sledge_hammer.c` | sledge hammer | 10800 | 1000 | twohanded blunt | 價值隨機 |
| 雷神之怒 | `blunt/thunder_gods_anger.c` | thunder god's anger | 26700 | 900 | twohanded blunt |  |
| 旋風流星槌 | `blunt/whirlwind_blunt.c` | whirlwind blunt | 26700 | 560 | twohanded blunt |  |
| 狼牙棒 | `blunt/wolf_hammer.c` | wolf hammer | 26700 | 450 | twohanded blunt | 價值隨機 |

## 槍

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 赤血槍 | `pike/blood_pike.c` | blood pike | 15700 | 850 | twohanded pike | 價值隨機 |
| 龍骨槍 | `pike/dragon_bone_pike.c` | dragon_bone pike | 18500 | 150 | twohanded pike |  |
| 鬼纓槍 | `pike/ghost_pike.c` | ghost pike | 23000 | 300 | twohanded pike |  |
| 紫金鳳頭錐 | `pike/gin_pike.c` | gin pike | 7900 | 50 | pike、secondhand pike | 劇毒、價值隨機 |
| 方天畫戟 | `pike/huge_halberd.c` | huge halberd | 20000 | 350 | twohanded pike | 價值隨機 |
| 巴蛇大矛 | `pike/hydra_lance.c` | hydra lance | 17100 | 500 | twohanded pike | 價值隨機 |
| 鑌鐵叉 | `pike/iron_fork.c` | iron fork | 4300 | 850 | pike | 價值隨機 |
| 渾鐵龍蛇槍 | `pike/iron_pike.c` | iron pike | 15700 | 200 | twohanded pike | 價值隨機 |
| 雷光戢 | `pike/lightning_pike.c` | lightning pike | 15700 | 350 | twohanded pike | 價值隨機 |
| 霸王槍 | `pike/royal_pike.c` | royal pike | 19200 | 200 | twohanded pike |  |
| 震天戢 | `pike/sky_pike.c` | sky pike | 4800 | 200 | pike、secondhand pike |  |
| 射日長槍 | `pike/solar_pike.c` | solar pike | 23000 | 700 | twohanded pike | 唯一 |
| 斷魂槍 | `pike/spear_of_slaying.c` | spear of slaying | 7300 | 50 | pike、secondhand pike | 價值隨機 |
| 精鋼短槍 | `pike/steel_short_spear.c` | steel short spear | 4800 | 750 | pike、secondhand pike | 價值隨機 |
| 紫電紅纓槍 | `pike/thunder_pike.c` | thunder pike | 17100 | 650 | twohanded pike | 價值隨機 |
| 虎鳴槍 | `pike/tiger_pike.c` | tiger pike | 7900 | 100 | pike、secondhand pike |  |
| 黃銅戰矛 | `pike/war_lance.c` | war lance | 17100 | 200 | twohanded pike | 價值隨機 |
| 憤怒明王槍 | `pike/wraith_of_warlord.c` | wraith of warlord | 9900 | 450 | pike、secondhand pike | 唯一、劇毒、價值隨機 |
| 夜叉戢 | `pike/yaksa_pike.c` | yaksa pike | 15700 | 150 | twohanded pike | 價值隨機 |

## 劍

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 玄武『青龍』劍 | `sword/armor_dragon_sword.c` | armor-dragon sword | 7200 | 50 | sword、secondhand sword |  |
| 古劍醒塵 | `sword/au_han_sword.c` | au-han sword | 9400 | 650 | sword | 唯一、價值隨機 |
| 黑色短劍 | `sword/black_shortsword.c` | black shortsword | 3200 | 450 | sword、secondhand sword | 價值隨機 |
| 黑劍 | `sword/black_sword.c` | black sword | 7200 | 100 | sword、secondhand sword | 價值隨機 |
| 玄蘇劍 | `sword/blackthorn.c` | blackthorn | 23900 | 300 | twohanded sword |  |
| 易羅大劍 | `sword/broadsword.c` | broadsword | 13000 | 600 | twohanded sword | 價值隨機 |
| 邪劍燹日 | `sword/catastrophe_sword.c` | catastrophe sword | 16900 | 900 | twohanded sword | 價值隨機 |
| 「穿靈」 | `sword/celestial_sword.c` | celestial sword | 2600 | 150 | sword | 價值隨機 |
| 『穿靈』 | `sword/celestial_sword_awakened.c` | celestial sword | 9400 | 100 | sword、secondhand sword | 價值隨機 |
| 櫻吹雪 | `sword/cherry_blossoms_sword.c` | cherry blossoms sword | 7200 | 400 | sword、secondhand sword | 價值隨機 |
| 紫炎劍 | `sword/cloud_magfire_sword.c` | cloud magfire sword | 6100 | 950 | sword | 價值隨機 |
| 落雲刃 | `sword/cloudslay_sword.c` | cloudslay sword | 8700 | 150 | sword、secondhand sword |  |
| 巨劍 | `sword/colosus_sword.c` | colosus sword | 16900 | 350 | twohanded sword | 價值隨機 |
| 古銅重劍 | `sword/copper_sword.c` | copper sword | 13000 | 650 | twohanded sword | 價值隨機 |
| 指南劍 | `sword/daoist_sword.c` | daoist sword | 3300 | 1000 | sword | 價值隨機 |
| 重劍龍吟 | `sword/dragon_heavy_sword.c` | dragon sword | 17900 | 650 | twohanded sword | 價值隨機 |
| 魚腸 | `sword/fishy_sword.c` | fishy sword | 9400 | 500 | sword、secondhand sword |  |
| 朱雀『白虎』劍 | `sword/flying_tiger_sword.c` | flying-tiger sword | 7900 | 550 | sword | 價值隨機 |
| 天都劍 | `sword/heaven_sword.c` | heaven sword | 7200 | 400 | sword、secondhand sword | 價值隨機 |
| 英雄劍 | `sword/hero_sword.c` | hero sword | 3300 | 300 | sword | 價值隨機 |
| 百鬒寒鳩劍 | `sword/hundreds_poison_sword.c` | hundreds_poison sword | 3300 | 350 | sword、secondhand sword | 劇毒 |
| 青鋼短劍 | `sword/iron_sword.c` | iron sword | 5500 | 950 | sword、secondhand sword | 價值隨機 |
| 冷玉寶劍 | `sword/jade_sword.c` | jade sword | 3300 | 600 | sword、secondhand sword | 價值隨機 |
| 殘煙劍 | `sword/legendary_sword_of_smoke.c` | legendary sword of smoke | 6200 | 250 | sword、secondhand sword | 價值隨機 |
| 藥王神劍 | `sword/medication_king_sword.c` | medication-king sword | 8700 | 1000 | sword |  |
| 雙鬢鋏 | `sword/needle_sword.c` | needle sword | 2500 | 400 | sword、secondhand sword | 價值隨機 |
| 水月劍 | `sword/night_sword.c` | night sword | 7200 | 100 | sword | 價值隨機 |
| 虛靈劍 | `sword/numinous_sword.c` | numinous sword | 7200 | 550 | sword | 唯一、價值隨機 |
| 桃木劍 | `sword/peachwood_sword.c` | peachwood sword | 2600 | 40 | sword |  |
| 靈通劍 | `sword/psychic_sword.c` | psychic sword | 4800 | 150 | sword、secondhand sword |  |
| 參龍『麒麟』劍 | `sword/pure_dragon_sword.c` | pure-dragon sword | 7900 | 100 | sword |  |
| 妖蛇『鳳凰』劍 | `sword/pure_phonix_sword.c` | pure-phonix sword | 7200 | 100 | sword、secondhand sword |  |
| 朱綸祭劍 | `sword/red_of_sword.c` | red of sword | 7200 | 350 | sword、secondhand sword | 價值隨機 |
| 少眉劍 | `sword/shome_sword.c` | shome sword | 3300 | 700 | sword | 價值隨機 |
| 洗銀劍 | `sword/silver_sword.c` | silver sword | 3300 | 800 | sword | 價值隨機 |
| 昊天大劍 | `sword/sky_sword.c` | sky sword | 17900 | 100 | twohanded sword | 價值隨機 |
| 繞指柔劍 | `sword/slasher_sword.c` | slasher sword | 3300 | 750 | sword | 價值隨機 |
| 巨劍『破石』 | `sword/smashing_sword.c` | smashing sword | 22000 | 350 | twohanded sword |  |
| 金蛇劍 | `sword/snake_sword.c` | snake sword | 7200 | 120 | sword |  |
| 精鋼長劍 | `sword/steel_longsword.c` | steel longsword | 3300 | 75 | sword |  |
| 風神之羽 | `sword/storm_lords_feather.c` | storm-lord's feather | 21200 | 500 | twohanded sword |  |
| 烏讎劍 | `sword/sword_of_black_justice.c` | sword of black justice | 7900 | 1500 | sword、secondhand sword |  |
| 命運之刃 | `sword/sword_of_destiny.c` | sword of destiny | 19000 | 450 | twohanded sword | 價值隨機 |
| 泣龍怨 | `sword/sword_of_dragon_tears.c` | sword of dragon tears | 16900 | 950 | twohanded sword | 價值隨機 |
| 落楓劍 | `sword/sword_of_falling_maple.c` | sword of falling maple | 7200 | 800 | sword、secondhand sword | 價值隨機 |
| 寒霜劍 | `sword/sword_of_frost_edge.c` | sword of frost-edge | 8700 | 150 | sword |  |
| 凝霜 | `sword/sword_of_holy_frost.c` | sword of holy frost | 16900 | 650 | twohanded sword | 價值隨機 |
| 蔑天劍 | `sword/sword_of_killing_god.c` | sword of killing god | 19000 | 250 | twohanded sword | 唯一、價值隨機 |
| 蠻龍 | `sword/sword_of_mighty_dragon.c` | sword of mighty dragon | 21200 | 250 | twohanded sword | 唯一、價值隨機 |
| 黃雀寶劍 | `sword/sword_of_raven.c` | sword of raven | 2600 | 450 | sword、secondhand sword | 唯一、價值隨機 |
| 咒劍紅羽 | `sword/sword_of_redrune.c` | sword of redrune | 3300 | 50 | sword |  |
| 風泉之劍 | `sword/sword_of_wind_spring.c` | sword of wind spring | 9400 | 800 | sword | 唯一、價值隨機 |
| 白龍牙 | `sword/white_dragon_fang.c` | dragon sword | 7900 | 500 | sword |  |
| 白色長劍 | `sword/white_sword.c` | white sword | 3300 | 50 | sword | 價值隨機 |
| 易羅大劍 | `sword/yiluo_sword.c` | yiluo sword | 16100 | 90 | twohanded sword |  |
| 儀陽劍 | `sword/yiyoung_sword.c` | yiyoung sword | 7200 | 500 | sword | 唯一 |

## 匕首（原資料列在劍類）

| 名稱 | 檔案 | 英文 | 重量 | 價值（兩） | 技能 | 備註 |
|---|---|---|---|---|---|---|
| 冰雪匕 | `dagger/dagger_of_frost_edge.c` | dagger of frost-edge | 8800 | 100 | dagger、secondhand dagger |  |
