# 天山（d/tianshan）設計文件

依使用者提供的新區域設計表（Excel，2026-11-14）製作，是四塊中的最後一塊（水月村 → 風城 → 孟家莊 → **天山**）。
天山：迷霧中有桃仙、柳家另一脈避世於此，天師四破雷法傳承的青雲觀也座落此（青雲觀、孤崖、山路屬風城區域，已在風城做好）。
房間敘述與出口絕大多數來自舊資料（`reference/es2_records/es2_pal/Quest--柳家拳內八套.txt`），全文引用。

## 連接

- 風城東城門 L12 往 `eastup` 接霧海 N10；霧海往 `westdown` 回東城門。
- 霧海 → 清泉：指令 `search 山路`（設計表寫 `msearch`，兩個都接受）；清泉往 south 回霧海（單向，霧海沒有往北的出口）。
- 清泉 → 山洞內 P8：`enter 縫隙`；山洞用 out 回清泉。
- 山洞內 R8 → 洞口 T8：`enter 洞口`；洞口往 west 回 R8。
- 洞口 T8 → 土路 V8：`search 小徑`；土路往 west 回洞口。
- 小木屋 X2 → 靈堂 V2：把供品（id `gong`，風城小販賣）交給柳清清（`give gong to liuh`），她把人送進內堂；靈堂 out 回小木屋。
- 靈堂 V2 → 密室 T2：`kneel 靈位`；密室 up 回靈堂。

## 房間（`map/area`：天山）

| 格子 | 檔案 | 名稱 | 來源 | 出口／指令 |
|---|---|---|---|---|
| N10 | `fog.c` | 霧海 | 舊資料全文 | westdown 東城門；`search 山路`／`msearch 山路` → 清泉 |
| N8 | `spring.c` | 清泉 | 設計表原文＋`look 縫隙` | south 霧海；`enter 縫隙` → 山洞 |
| P8 | `cave1.c` | 山洞內 | 舊資料全文（圖層「洞穴」） | out 清泉、east R8 |
| R8 | `cave2.c` | 山洞內 | 舊資料全文（圖層「洞穴」）；舊文「光亮從西邊的小洞口照進來」保留不改 | west P8；`enter 洞口` → 洞口 |
| T8 | `cave_mouth.c` | 洞口 | 舊資料全文（刪「的」避免標點凸出）＋`look 桃花叢` | west R8；`search 小徑` → 土路 |
| V8 | `dirt_road1.c` | 土路 | 舊資料全文 | west T8、east X8 |
| X8 | `fork.c` | 岔路口 | 舊資料全文 | north X6、west V8、south X10、east Z8 |
| Z8 | `peach_grove.c` | 桃花林 | 舊資料全文 | west X8 |
| X6 | `dirt_road2.c` | 土路 | 新寫 | north X4、south X8 |
| X4 | `dirt_road3.c` | 土路 | 新寫 | north X2、south X6 |
| X2 | `cabin.c` | 小木屋 | 舊資料全文 | south X4；給供品 → 靈堂 |
| X10 | `garden.c` | 小菜園 | 新寫 | north X8 |
| V2 | `hall.c` | 靈堂 | 舊資料全文＋`look 靈位`（圖層「內堂」） | out 小木屋；`kneel 靈位` → 密室 |
| T2 | `secret_room.c` | 密室 | 舊資料全文＋`look 字跡`（圖層「密室」） | up 靈堂；`wipe 灰塵` |

## 密室與柳家拳內八套（使用者決定，2026-11-14：先做房間）

- 舊資料的流程：`look 字跡`（灰塵太多，可以抹去）→ `wipe 灰塵`（牆上有一些內八套的武功招式，可以學學看）→ `learn 內八套`。
- 舊資料的學習條件：封山派弟子、柳家拳法 50 級。
- **柳家拳內八套（`intrinsic-liuh-ken`）技能還沒有設計**，這次只做房間與 `wipe`；`learn 內八套` 目前只回覆「卻怎麼也看不出其中的門道」，不傳授任何技能。
- 之後設計好內八套，再把 `secret_room.c` 的 `do_learn()` 接上傳授。

## NPC 與物品

| NPC | 位置 | 說明 |
|---|---|---|
| 桃仙 | Z8 桃花林 | 現有 `custom/legacy_npc/mountain_god.c`（人類 30 級、強度 B）；**身上帶【脈穴內經】**（`obj/books/book_of_anatomy`，使用者指示 2026-11-14） |
| 柳清清 | X2 小木屋 | `npc/liuh_qingqing.c`；人類 18 歲、柳家後人；玩家走進來她說「你好啊，我最近要祭奠我爺爺了。」；收供品送玩家進內堂 |

## 網頁地圖

`web/world_static_map.json` 加入 14 間房（延續風城的座標：N10 在 x=7，往東每兩個欄位一格；X 欄 x=12）；山洞（洞穴）、靈堂（內堂）、密室各自在獨立圖層。

## 驗證

- `tests/test_area_tianshan.py`：房間數、區域與圖層、出口對照舊資料、指令出入、密室 learn 尚未開放、NPC 與桃仙的脈穴內經、敘述版面、網頁地圖。
- `python3 tools/check_big5_text.py`：0 處。
- 尚未實機載入遊戲跑過；上線前請巫師走一遍所有出口與指令出入。

## 待辦

柳家拳內八套的設計與傳授。四塊新區域（水月村、風城、孟家莊、天山）至此全部完成。
