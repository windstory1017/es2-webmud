# 五堂鎮（d/wutang）

依使用者提供的五堂鎮設計表（Google 試算表）製作：房間 56、NPC 31 種（之後另加冷梅莊莊主梅影風，共 32 種）。

## 規則

- **房間名稱**照設計表；**敘述**：「房間敘述擴充 N」照原文，「Y」依需求與周圍房間擴寫成約 3～4 行（遊戲顯示寬度）。敘述寫成一整段，由遊戲的 look 自己縮排換行（同李家村）。
- **對外**：五堂鎮口往北接振武軍營大門（大門加上 south）。往喬陰縣、水嵐縣只寫在敘述中，沒有出口。
- **指令出入**：鎮天神廟 `enter 後院`／後院 `out`；廟口小路(X6) `enter 暗巷` 到前門，前門 `east` 回來；顏家大宅 `in`／大廳 `out`；廟口小路(X8) `enter 小巷子` 到暗巷，暗巷 `east` 回來；碎石路與綠竹林互相 `pass 草叢`；河邊與小船上互相 `swim`；鯉君渡 `northdown` 到鯉君渡口，渡口 `southup` 回來。
- **安全區**（不能戰鬥）：和豐當鋪、醇雨樓客棧（旅館本身即是）、城隍廟、河邊、鯉君渡口。
- **功能**：和豐當鋪為當鋪（HOCKSHOP）、景隆錢莊為錢莊（BANK）、醇雨樓客棧為旅館（INN）。店小二賣水餃、饅頭、豬肉、烤雞；賣餅大叔賣餡餅（每次進貨 20 個）；珍寶商人賣漢玉戒指、寒玉戒指、黃玉戒指。
- **野獸種族**（`daemon/race/beast.c`）：只給 NPC 用，不在建立角色的種族清單裡；不能裝備，徒手攻擊改用咬、撞、踢（`daemon/skill/beast.c`）。野豬、大野豬主動攻擊玩家，受傷後有 20% 機會逃往相鄰格子（實測 43/200）。
- **城隍爺**（Town god）：鬼魂，只有陰陽眼看得到（玩家死後的鬼魂本身就有）；鬼魂 `register` 即復活，規則同雪亭鎮小廟的紅色漩渦（`CHAR_D->make_living`）；活人 register 會被告知陽壽未盡。
- **NPC 數值**：照設計表；沒寫技能的依等級給徒手、閃躲、招架（等級 × 2），有兵器的加兵器技能；沒寫精氣神的依等級換算（武者／平民：精 12、氣 15、神 5 倍等級；道士 12、13、20；書生 10、10、15；最少 35）。蝶影幻步（butterfly-steps）先佔位 110。表上錯字修正：taoism_freezn → taoism-freeze，spelles → spells。
- **閒聊**：楊子陵、雍泰、賣餅大叔、陸四爺、小鼠兒、老火、廟祝、老人、李山、葉翔各兩句，每 tick 16%（約 5～7 tick 一句）。
- **新物品**：青布衣（`d/wutang/obj/blue_cloth.c`，防禦 4，價值 300 文）、餡餅（`d/wutang/obj/pie.c`，價值 30 文）。布衣（`obj/area/obj/cloth.c`）價值由 0 改為 200 文。
- **任務**（蝶影幻步、慈嚴除障手、追風劍、六靈玄衣、風嘯拳法、趙欽差等）之後再做。

## 房間

| 格子 | 檔案 | 名稱 | 敘述 | NPC |
|---|---|---|---|---|
| X4 | `temple.c` | 鎮天神廟 | 原文 | 廟祝 |
| Z4 | `temple_yard.c` | 後院 | 原文 | 老人 |
| X6 | `temple_road_n.c` | 廟口小路 | 擴寫 |  |
| Z6 | `path_e.c` | 小路 | 擴寫 |  |
| Z8 | `backyard.c` | 後院 | 原文 |  |
| X8 | `temple_road_s.c` | 廟口小路 | 擴寫 |  |
| X10 | `market_square.c` | 雲棧市集廣場 | 原文 | 葉翔 |
| V4 | `yan_hall.c` | 大廳 | 擴寫 |  |
| V6 | `yan_mansion.c` | 顏家大宅 | 擴寫 |  |
| V8 | `yan_gate.c` | 前門 | 擴寫 |  |
| T8 | `dark_alley_e.c` | 暗巷 | 原文 | 李山 |
| R8 | `dark_alley_w.c` | 暗巷 | 擴寫 |  |
| N8 | `entrance.c` | 五堂鎮口 | 擴寫 | 小鼠兒、老火 |
| J10 | `school.c` | 鄉校 | 擴寫 |  |
| L10 | `pawnshop.c` | 和豐當鋪 | 原文 | 陸四爺 |
| N10 | `north_street.c` | 五堂鎮 | 擴寫 |  |
| R10 | `inn.c` | 醇雨樓客棧 | 原文 | 店小二、雍泰 |
| R10（二樓） | `inn2.c` | 醇雨樓客棧二樓 | 原文 | 白衣道士 ×3、紅衣道士 ×3 |
| R10（三樓） | `inn3.c` | 醇雨樓客棧三樓 | 原文 | 珍寶商人、青年書生 |
| T10 | `cloth_shop.c` | 布莊 | 擴寫 | 小桃、青年公子 |
| F12 | `pavilion.c` | 涼亭 | 擴寫 |  |
| H12 | `gravel_road_n.c` | 碎石路 | 擴寫 | 楊子陵、冷梅莊二代弟子 |
| J12 | `west_street2.c` | 街道 | 擴寫 |  |
| L12 | `west_street1.c` | 街道 | 擴寫 |  |
| N12 | `crossroad.c` | 交叉路口 | 原文 | 賣餅大叔、郭布（梅影風已搬回冷梅莊正廳，2026-10-30） |
| P12 | `east_street1.c` | 街道 | 擴寫 |  |
| R12 | `east_street2.c` | 街道 | 擴寫 |  |
| T12 | `east_street3.c` | 街道 | 擴寫 |  |
| V12 | `east_street4.c` | 街道 | 擴寫 |  |
| X12 | `city_god_temple.c` | 城隍廟 | 擴寫 | 城隍爺 |
| F14 | `gravel_road_s.c` | 碎石路 | 擴寫 |  |
| H14 | `bamboo_hall.c` | 主廳 | 原文 | 呼延光 |
| D14 | `riverside.c` | 河邊 | 原文 | 老駱、小黃 |
| N14 | `south_street1.c` | 街道 | 擴寫 |  |
| P14 | `bank.c` | 景隆錢莊 | 擴寫 | 護院武師 ×4 |
| T14 | `guesthouse.c` | 五堂別館 | 原文 | 趙欽差 |
| D16 | `boat.c` | 小船上 | 擴寫 | 煙波釣叟 |
| H16 | `bamboo_grove.c` | 綠竹林 | 擴寫 |  |
| N16 | `south_street2.c` | 街道 | 擴寫 |  |
| N18 | `three_way.c` | 三岔路口 | 擴寫 |  |
| J14 | `grass_nw.c` | 草原 | 擴寫 | 綿羊 ×2 |
| L14 | `grass_ne.c` | 草原 | 擴寫 | 綿羊 ×2、牧羊人 |
| J16 | `grass_sw.c` | 草原 | 擴寫 | 綿羊 ×2 |
| L16 | `grass_se.c` | 草原 | 擴寫 | 綿羊 ×2、牧羊人 |
| H18 | `ferry.c` | 鯉君渡 | 擴寫 |  |
| J18 | `boardwalk_w.c` | 木道 | 擴寫 |  |
| L18 | `boardwalk_e.c` | 木道 | 擴寫 |  |
| H20 | `ferry_dock.c` | 鯉君渡口 | 原文 | 釣客 ×2 |
| J22 | `river_bank.c` | 羿水河邊 | 擴寫 |  |
| B6 | `hut.c` | 草屋 | 擴寫 |  |
| D6 | `field1.c` | 菜田 | 擴寫 | 大野豬、野豬 ×2 |
| D8 | `field2.c` | 菜田 | 擴寫 | 野豬 ×3 |
| F8 | `field3.c` | 菜田 | 擴寫 | 野豬 ×3 |
| D10 | `field4.c` | 菜田 | 擴寫 | 野豬 ×3 |
| F10 | `field5.c` | 菜田 | 擴寫 | 野豬 ×2 |
| H10 | `clearing.c` | 空地 | 擴寫 |  |

## NPC

| 檔案 | 名稱 | 種族 | 職業 | 等級 | 裝備 |
|---|---|---|---|---|---|
| `npc/keeper.c` | 廟祝 | human | commoner | 1 | cloth |
| `npc/oldman.c` | 老人 | human | commoner | 1 | cloth |
| `npc/yaesae.c` | 玉面好漢 葉翔 | human | thief | 30 |  |
| `npc/lee_shan.c` | 李山 | human | commoner | 5 |  |
| `npc/young_man.c` | 小鼠兒 | human | commoner | 1 |  |
| `npc/old_fire.c` | 老火 | human | commoner | 1 |  |
| `npc/lu_estimator.c` | 陸四爺 | human | commoner | 1 |  |
| `npc/waiter.c` | 店小二 | human | commoner | 1 |  |
| `npc/yung_tai.c` | 雍泰 | yenhold | fighter | 20 |  |
| `npc/white_taoist.c` | 白衣道士 | woochan | taoist | 15 | white_taoist_robe、longsword |
| `npc/red_taoist.c` | 紅衣道士 | human | taoist | 15 | red_robe、longsword |
| `npc/jeweller.c` | 珍寶商人 | human | commoner | 10 |  |
| `npc/young_scholar.c` | 青年書生 | human | commoner | 10 | jeweled_shortsword、blue_cloth、anthology_of_classical_prose |
| `npc/tao.c` | 小桃 | human | commoner | 5 |  |
| `npc/young_gentleman.c` | 青年公子 | human | commoner | 10 |  |
| `npc/seller.c` | 賣餅大叔 | human | commoner | 1 |  |
| `npc/guo_boo.c` | 朱衣派道士 郭布 | human | taoist | 60 | red_robe、red_hat、sword_of_redrune |
| `npc/ro.c` | 老駱 | human | commoner | 1 |  |
| `npc/huang.c` | 小黃 | human | commoner | 1 |  |
| `npc/sheep.c` | 綿羊 | beast | — | 1 |  |
| `npc/shepherd.c` | 牧羊人 | human | commoner | 1 |  |
| `npc/fisher.c` | 釣客 | human | commoner | 1 |  |
| `npc/smoke_fisher.c` | 煙波釣叟 | human | fighter | 50 | silk_fishing_stick、straw_cloth |
| `npc/big_boar.c` | 大野豬 | beast | — | 1 |  |
| `npc/boar.c` | 野豬 | beast | — | 1 |  |
| `npc/royalist.c` | 趙欽差 | human | soldier | 1 |  |
| `npc/guard.c` | 護院武師 | blackteeth | fighter | 20 | cowhide_vest |
| `npc/town_god.c` | 城隍爺 | human | commoner | 1 |  |
| `npc/yang_zlin.c` | 冷梅莊一代弟子 楊子陵 | human | fighter | 45 | black_sword、black_iron_dagger、silk_cloth、wolf_boots、woof_hat |
| `npc/apprentice.c` | 冷梅莊二代弟子 | human | fighter | 25 |  |
| `npc/huyen_guan.c` | 綠竹居士 呼延光 | human | scholar | 35 | white_cloth |
