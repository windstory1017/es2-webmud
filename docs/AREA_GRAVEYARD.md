# 新區域：亂葬崗

> 依使用者提供的 Excel（2026-11-04）與討論結果製作。
> 目錄：`source/upstream/mudlib/d/graveyard/`（房間 13、新 NPC 4 種、新物品 4 樣），區域名稱「亂葬崗」。
> 從雪亭鎮西門（`/d/snow/wgate`）往西北（nw）進入，回程往東南（se）。

## 地圖

```
 祠堂(F2) ┄enter 小祠／out┄ 亂葬崗(H2) ─ 亂葬崗(J2)
                           ╱                  ╲
                  亂葬崗(F4)                    亂葬崗(L4)
                     │                                  ╲
       亂葬崗(D6) ─ 亂葬崗(F6)                        亂葬崗(N6) ─ 亂葬崗(P6) ─ 東南：雪亭鎮西門
          ┆ pass 草叢（回程 north）
       雜草叢(D8)
          │
 密林(B10) ─ 密林(D10) ─ 密林(F10)
```

## 房間

| 格子 | 檔名 | 名稱 | 出口 | 指令出入 | NPC | 敘述來源 |
|---|---|---|---|---|---|---|
| P6 | `grave1` | 亂葬崗 | w N6、se 雪亭鎮西門 | — | — | 新寫 |
| N6 | `grave2` | 亂葬崗 | e P6、nw L4 | — | — | 新寫 |
| L4 | `grave3` | 亂葬崗 | nw J2、se N6 | — | — | 新寫 |
| J2 | `grave4` | 亂葬崗 | w H2、se L4 | — | — | 新寫（設計表未列，使用者同意補上） |
| H2 | `grave5` | 亂葬崗 | sw F4、e J2 | `enter 小祠` → F2 | 青衣女郎 | 舊資料（風嘯拳法） |
| F2 | `shrine` | 祠堂 | out H2 | — | 邋遢道士 | 舊資料（風嘯拳法） |
| F4 | `grave6` | 亂葬崗 | ne H2、s F6 | — | — | 新寫 |
| F6 | `grave7` | 亂葬崗 | n F4、w D6 | — | 老和尚、小和尚 | 新寫（寫進 D6 舊文提到的破敗石亭） |
| D6 | `grave8` | 亂葬崗 | e F6 | `pass 草叢` → D8 | — | 舊資料（柳家拳法），出口只有 e |
| D8 | `weeds` | 雜草叢 | n D6、s D10 | — | — | 舊資料 |
| D10 | `forest` | 密林 | n D8、w B10、e F10 | — | — | 新寫 |
| B10 | `forest_w` | 密林 | e D10 | — | 吳長貴 | 新寫 |
| F10 | `forest_e` | 密林 | w D10 | — | 野人酋長、野人 ×2 | 新寫 |

- `grave2` 原在 N4（P6 往西北），使用者調整位置：先移除（2026-11-05），再放回 N6，P6 往西接 N6、N6 往西北接 L4（使用者指示，2026-11-05）。
- 祠堂以外都是戶外（`outdoors`），畫面會出現天色。
- D6 往南沒有一般出口，要 `pass 草叢`；雜草叢往北是一般出口，直接回 D6。
- 舊資料全文引用、未改字。D6 舊文的最後一行只有「林。」，使用者同意照原文。
- 密林 B10、D10、F10 原設計表名為「雜草叢」，使用者改為「密林」（2026-11-04）；正好對上 D8 舊文「南邊是一處密林」。

## NPC

| NPC | 檔案 | 職業 | 程度 | 強度 | 裝備 |
|---|---|---|---|---|---|
| 邋遢道士 (Dirty taoist) | `d/graveyard/npc/dirty_taoist.c` | 道士 | 10 | C | 紫色道袍、貓眼戒指、骨灰罈 |
| 老和尚 (Old bonze) | `d/graveyard/npc/old_bonze.c` | 和尚 | 20 | B | 黃銅禪杖、念珠 |
| 小和尚 (Young bonze) | `d/graveyard/npc/young_bonze.c` | 和尚 | 1 | C | 念珠 |
| 野人 (Barbarian) ×2 | `d/graveyard/npc/barbarian.c` | 武者 | 12 | B | —（膂力 20） |
| 青衣女郎 (Lady) | `custom/legacy_npc/lady.c` | 盜賊 | 5 | B | 匕首（既有） |
| 吳長貴 (Wu changuei) | `custom/legacy_npc/wu_changuei.c` | 盜賊 | 15 | B | 既有 |
| 野人酋長 (Barbarian chief) | `custom/legacy_npc/barbarian_chief.c` | 武者 | 15 | A | 白骨杖；看到玩家就下殺手 |

- 野人酋長原為程度 5、C 級；使用者要求酋長比野人強，調為程度 15、A 級（2026-11-04）。
- 野人設計表沒有寫主動攻擊，所以不主動攻擊。

## 物品

| 物品 | 檔案 | 內容 |
|---|---|---|
| 貓眼戒指 (Chrysoberyl ring) | `obj/chrysoberyl_ring.c` | 戒指，定力 +1；敘述提到指環內刻「雍順」 |
| 骨灰罈 (Urn) | `obj/urn.c` | 一般物品 |
| 黃銅禪杖 (Brass zenstaff) | `obj/brass_zenstaff.c` | 以禪杖為基礎，重量 18840、價值 72000（各 ×1.2），慧根 +1、守勢等級 +5；照禪杖一樣擲隨機附加屬性 |
| 念珠 (Rosary) | `obj/rosary.c` | 項鍊，悟性 +1，重量 20，價值 1200 |

「雍順」與骨灰罈是風嘯拳法任務的線索（2026-11-05）：邋遢道士喝酒會醉倒，醉倒時可以搜身，也可以殺了他拾取。任務流程見 `docs/martial_arts/共通_風嘯拳法.md`。

## 驗證

- 實際開機，從雪亭鎮西門走完全部房間與兩個指令出入，`look` 看過每間的排版，沒有標點凸出；
  看過各 NPC 與新物品；紀錄檔沒有程式錯誤。
- 測試：`tests/test_area_graveyard.py`（出口、指令出入、NPC 擺放、物品數值、敘述排版、網頁地圖）。
- 網頁地圖（`web/world_static_map.json`）與天朝圖鑑（`tools/build_codex.py` 的 `MAP_POS`）依設計表格位加入。
