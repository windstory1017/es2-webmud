# 新區域：康平村、百花村

> 依使用者提供的 Excel（2026-10-20）與討論結果製作。
> 康平村：`source/upstream/mudlib/d/kangping/`（房間 7、NPC 2）；百花村：`d/baihua/`（房間 15、NPC 12）。
> 兩村都從老松林進入，Excel 上黃底格子（B4 空地、B6 草叢、D8 草叢、F8 山洞）是既有房間，只作定位用。

## 地圖

### 康平村（綠底）

```
                    康平村 F4 ─ 谷口 H4 ─ 小客店 J4
                       │          │
草叢 B6 ─ 樹林口 D6 ─ 康平村 F6    廣場 H6 ─ 草廬 J6
（老松林 grass1）
```

- 老松林草叢（`/d/oldpine/grass1`）新增往東出口到樹林口。
- 谷口往北是三煙谷，尚未開放，只在敘述中帶到，不開出口。
- 康平村 F6 與廣場 H6 不相連；廣場只能從谷口往南進入。

### 百花村（藍底）

```
                懸崖平台 H8
                    ↕（百花崖 climb 峭壁上去，平台 eastup 回來）
草叢 D8         百花崖 H10 ─ 瓦屋 J10 ─ 石子路 L10 ─ 峭壁邊 N10
（老松林 grass2）                            │
   ╲                         學堂 J12 ─ 石子路 L12 ─ 老王的家 N12
  山腰上 F10                                 │
   ↕（search 草叢；小徑往西北回來）            │
荒野小徑 J14 ──────────────────── 常春道 L14 ─ 小巷 N14
                                             │
                                          常春道 L16 ─ 紅燈客棧 N16
                                         ╱
                                  破舊小廟 J18
```

- 老松林草叢（`/d/oldpine/grass2`）新增往東南出口到山腰上。草叢原本的 cave 進山洞、山洞 out 回草叢不變。
- 山腰上與百花崖不相連；進村要在山腰上搜草叢（search 草叢）找到荒野小徑。
  只打 search 或搜別的東西時顯示：「到處都是比人還高的草叢，仔細找找或許可以找到出路。」（2026-10-24）
- Excel 上的 H12「草叢」是製作過程沒刪到的列，不做成房間（使用者確認）。

## 房間

| 格子 | 檔案 | 房間 | 敘述 | NPC |
|---|---|---|---|---|
| D6 | `kangping/forest_exit` | 樹林口 | 撰寫 | — |
| F6 | `kangping/village_gate` | 康平村（村口） | 撰寫 | — |
| F4 | `kangping/village` | 康平村（村內） | 撰寫 | — |
| H4 | `kangping/valley_mouth` | 谷口 | 撰寫（三煙谷、野狼） | — |
| J4 | `kangping/inn` | 小客店（非戰區、可存檔） | 沿用，「往東／往北」改為「往西」 | 歐陽無極、柳淳風、客店主人 |
| H6 | `kangping/square` | 廣場 | 撰寫（古井） | 阿桂 |
| J6 | `kangping/hut` | 草廬 | 沿用（房名「草蘆」改「草廬」，與敘述一致） | 安懶道人 |
| F10 | `baihua/hillside` | 山腰上 | 沿用 | 山羊 ×3（`/d/oldpine/npc/goat`，2026-10-24） |
| H8 | `baihua/platform` | 懸崖平台 | 沿用 | 高玉 |
| H10 | `baihua/cliff` | 百花崖 | 沿用 | 杜福 |
| J10 | `baihua/tile_house` | 瓦屋 | 沿用 | 王燕 |
| L10 | `baihua/stone_road1` | 石子路 | 撰寫 | — |
| N10 | `baihua/cliff_edge` | 峭壁邊 | 撰寫（往下眺望） | — |
| J12 | `baihua/school` | 學堂 | 撰寫 | 小學童 ×3 |
| L12 | `baihua/stone_road2` | 石子路 | 撰寫 | — |
| N12 | `baihua/wang_home` | 老王的家 | 沿用 | 老王、牧童 |
| J14 | `baihua/trail` | 荒野小徑 | 沿用 | — |
| L14 | `baihua/road1` | 常春道 | 撰寫 | — |
| N14 | `baihua/alley` | 小巷 | 沿用 | 阿三哥、小姑娘 |
| L16 | `baihua/road2` | 常春道 | 沿用 | 南宮恨、路人 |
| N16 | `baihua/inn` | 紅燈客棧（非戰區、可存檔） | 沿用 | 公孫志、掌櫃、過路客 |
| J18 | `baihua/temple` | 破舊小廟 | 撰寫 | 石獅子 |

- 撰寫的敘述都是三到四行、最後一行至少六字，不寫單純指路的句子。
- 沿用的敘述修正錯字：山風「括」→「颳」、「開的」→「開得」、「變的」→「變得」、「招乎」→「招呼」。

## NPC

| NPC | 檔案 | 職業／等級／強度 | 備註 |
|---|---|---|---|
| 「穿雲槍」高玉 | `baihua/npc/kao_yeu` | 武者 35 B | 受重傷寫在敘述裡；穹槍訣（`sky_pike`）尚無武功檔，只給名稱與等級；金羽神盔、龍骨槍、鎖子甲、封神靴 |
| 「花中菩提」王燕 | `baihua/npc/wang_yean` | 方士 20 B | 龍圖丹派（`custom_faction` 記號，不顯示稱號）；翠綠短杉、神農匕、神羊帽；敘述錯字修正 |
| 杜福 | `baihua/npc/dou_foo` | 武者 20 B | 雙手杖法；辛木杖 |
| 小學童 ×3 | `baihua/npc/student` | 平民 3 C | 沿用敘述 |
| 老王 | `baihua/npc/old_wang` | 平民 1 C | 沿用敘述 |
| 牧童 | `baihua/npc/shepherd` | 平民 1 C | 牧羊杖 |
| 阿三哥 | `baihua/npc/san` | 平民 1 C | 壯碩青年；年齡表上寫 8，依「青年」改為 18 |
| 小姑娘 | `baihua/npc/girl` | 平民 1 C | 做羹湯照顧村民、喜歡阿三哥；年齡表上寫 4，改為 14 |
| 「落山風」南宮恨 | `baihua/npc/nan_kong_hen` | 道士 40 A | 玄衣派（`taoist.storm`） |
| 掌櫃 | `baihua/npc/innkeeper` | 平民 1 C | 紅燈客棧，櫃台打算盤 |
| 過路客 | `baihua/npc/traveller` | — | 複製 `obj/area/traveller.c`，改拿路人的短劍（產生時擲隨機附加屬性） |
| 石獅子 | `baihua/npc/stone_lion` | — | 比照振武軍營假人（不還手、不損壞，命中敘述不提部位） |
| 客店主人 | `kangping/npc/innkeeper` | 平民 1 C | 小客店，一人兼掌櫃與店小二 |
| 阿桂 | `kangping/npc/kid` | 平民 1 C | 熱心的小孩 |

沿用既有檔案：公孫志、安懶道人、歐陽無極、柳淳風（`custom/legacy_npc/`）、路人（`obj/area/man.c`）。

**兩間客店販售**：紅燒雞 30、牛肉包子 20、白切肉 10（表上「燒雞」用遊戲既有的紅燒雞）。

## 新武器

| 武器 | 檔案 | 設定 |
|---|---|---|
| 辛木杖 (Sinia staff) | `custom/weapon/staff/sinia_staff.c` | 強度同木杖（雙手杖 3-12），另加防禦力 5、防禦能力值 25；價值 100 兩銀子 |
| 牧羊杖 (Cane) | `custom/weapon/staff/shepherd_cane.c` | 強度同短劍，單手杖，價值 1000 文 |

## 網頁地圖

兩村各自一套座標（每兩格一單位）：康平村以樹林口 D6 為原點，百花村以山腰上 F10 為原點。

## 驗證（Neolith 實際載入，2026-10-20）

- 22 個房間全部走過，所有出口互通；老松林兩個草叢往東、往東南都能進入。
- 山腰上 search 草叢到荒野小徑、百花崖 climb 峭壁上懸崖平台、平台 eastup 回百花崖都正常。
- 每個 NPC 都在指定位置，裝備正確；過路客的短劍實測擲出附加屬性。
- 兩間客店商品正確；石獅子被攻擊不還手、敘述不提部位。
- 測試：`tests/test_area_kangping_baihua.py`。
