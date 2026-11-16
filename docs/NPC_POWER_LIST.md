# 現有 NPC 強度清單（已套用）

> 依使用者確認的表格套用 `set_power()`（規則見 `CUSTOM_NPC_POWER.md`）。
> 套用的 NPC 已移除檔案中手動寫死的屬性與精氣神上限，改由系統計算。
> 野獸改用 `set_beast(1～10)`（數值表見 `daemon/race/beast.c`，數值不變；綿羊新設強度 2）。
> 鬼魂（含城隍爺，平民 LV1 鬼魂狀態，由 setup_ghost() 設定）、練功假人維持原樣。

| # | 地點 | 名稱 | 檔案 | 職業 | 等級 | 強度 |
|---|---|---|---|---|---|---|
| 1 | 鬼怪 | 餓鬼 | `custom/ghost/npc/hungry_ghost.c` | — | — | 不動（鬼魂） |
| 2 | 鬼怪 | 魑魅 | `custom/ghost/npc/mountain_sprite.c` | — | — | 不動（鬼魂） |
| 3 | 鬼怪 | 倀鬼 | `custom/ghost/npc/tiger_thrall.c` | — | — | 不動（鬼魂） |
| 4 | 鬼怪 | 遊魂 | `custom/ghost/npc/wandering_soul.c` | — | — | 不動（鬼魂） |
| 5 | 鬼怪 | 魍魎 | `custom/ghost/npc/water_goblin.c` | — | — | 不動（鬼魂） |
| 6 | 鬼怪 | 枉死鬼 | `custom/ghost/npc/wronged_ghost.c` | — | — | 不動（鬼魂） |
| 7 | 家園 | （butler） | `custom/home/npc/butler.c` | 平民 | 1 | C |
| 8 | 天師道 | 朱衣派弟子 | `custom/taoism/npc/fire_taoist.c` | 道士 | 30 | B |
| 9 | 天師道 | 素衣派弟子 | `custom/taoism/npc/freeze_taoist.c` | 道士 | 30 | B |
| 10 | 天師道 | 玄衣派弟子 | `custom/taoism/npc/storm_taoist.c` | 道士 | 30 | B |
| 11 | 天師道 | 紫衣派弟子 | `custom/taoism/npc/thunder_taoist.c` | 道士 | 30 | B |
| 12 | 振武營 | 魯熙年 | `custom/zhenwu/npc/lu_xinien.c` | 軍人 | 25 | B |
| 13 | 振武營 | 米沛 | `custom/zhenwu/npc/mee_pei.c` | 軍人 | 40 | A |
| 14 | 振武營 | 小兵 | `custom/zhenwu/npc/soldier.c` | 軍人 | 5 | B |
| 15 | 振武營 | 假人 | `custom/zhenwu/npc/target_stake.c` | — | — | 不動（練功假人） |
| 16 | 振武營 | 振武軍營士兵 | `custom/zhenwu/npc/zhenwu_soldier.c` | 軍人 | 35 | C |
| 17 | 李家村 | 小孩 | `d/lee/npc/child.c` | 平民 | 1 | C |
| 18 | 李家村 | 農夫 | `d/lee/npc/farmer.c` | 平民 | 5 | C |
| 19 | 李家村 | 守衛 | `d/lee/npc/guard.c` | 武者 | 15 | C |
| 20 | 李家村 | 冰糖葫蘆小販 | `d/lee/npc/hawthorn_seller.c` | 平民 | 5 | C |
| 21 | 李家村 | 小藥童 | `d/lee/npc/herb_boy.c` | 平民 | 3 | C |
| 22 | 李家村 | 獵戶 | `d/lee/npc/hunter.c` | 武者 | 5 | B |
| 23 | 李家村 | 李勖賢 | `d/lee/npc/lee_hsu_hsien.c` | 書生 | 10 | C |
| 24 | 李家村 | 李嘯天 | `d/lee/npc/lee_xiao_tian.c` | 武者 | 20 | B |
| 25 | 李家村 | 李岩 | `d/lee/npc/lee_yen.c` | 平民 | 4 | C |
| 26 | 李家村 | 李勇 | `d/lee/npc/lee_yong.c` | 武者 | 15 | B |
| 27 | 李家村 | 李忠 | `d/lee/npc/lee_zhong.c` | 平民 | 5 | C |
| 28 | 李家村 | 李半仙 | `d/lee/npc/li_ban_xian.c` | 平民 | 5 | C |
| 29 | 李家村 | 聶晟 | `d/lee/npc/nee_cheng.c` | 方士 | 40 | A |
| 30 | 李家村 | 私塾先生 | `d/lee/npc/teacher.c` | 平民 | 5 | C |
| 31 | 李家村 | 旅客 | `d/lee/npc/traveller.c` | 平民 | 5 | C |
| 32 | 李家村 | 店小二 | `d/lee/npc/waiter.c` | 平民 | 2 | C |
| 33 | 李家村 | 婦人 | `d/lee/npc/woman.c` | 平民 | 5 | C |
| 34 | 迷霧森林 | 土匪 | `d/oldpine/npc/bandit.c` | 盜賊 | 10 | C |
| 35 | 迷霧森林 | 土匪嘍囉 | `d/oldpine/npc/bandit_minion.c` | 盜賊 | 5 | C |
| 36 | 迷霧森林 | 大黑熊 | `d/oldpine/npc/big_bear.c` | 野獸 | — | 野獸強度 5 |
| 37 | 迷霧森林 | 大觀和尚 | `d/oldpine/npc/da_guan.c` | 和尚 | 30 | A |
| 38 | 迷霧森林 | 野鹿 | `d/oldpine/npc/deer.c` | 野獸 | — | 野獸強度 3 |
| 39 | 迷霧森林 | 採藥人 | `d/oldpine/npc/herbalist.c` | 平民 | 1 | C |
| 40 | 迷霧森林 | 受傷的旅客 | `d/oldpine/npc/injured_traveller.c` | 平民 | 1 | C |
| 41 | 迷霧森林 | 高慎 | `d/oldpine/npc/kao_shen.c` | 武者 | 30 | B |
| 42 | 迷霧森林 | 靈玉嬋 | `d/oldpine/npc/lin_yuchan.c` | 武者 | 30 | A |
| 43 | 迷霧森林 | 小黑熊 | `d/oldpine/npc/little_bear.c` | 野獸 | — | 野獸強度 4 |
| 44 | 迷霧森林 | 老鼠 | `d/oldpine/npc/rat.c` | 野獸 | — | 野獸強度 1 |
| 45 | 迷霧森林 | 旅行小販 | `d/oldpine/npc/seller.c` | 平民 | 1 | C |
| 46 | 迷霧森林 | 松鼠 | `d/oldpine/npc/squirrel.c` | 野獸 | — | 野獸強度 1 |
| 47 | 迷霧森林 | 店小二 | `d/oldpine/npc/waiter.c` | 平民 | 1 | C |
| 48 | 迷霧森林 | 凶暴的野熊 | `d/oldpine/npc/wild_bear.c` | 野獸 | — | 野獸強度 7 |
| 49 | 迷霧森林 | 土狼 | `d/oldpine/npc/wolf.c` | 野獸 | — | 野獸強度 4 |
| 50 | 迷霧森林 | 徐彪 | `d/oldpine/npc/xue_biao.c` | 盜賊 | 20 | B |
| 51 | 雪亭鎮 | 陳維俠 | `d/snow/npc/alchemist.c` | 方士 | 30 | A |
| 52 | 雪亭鎮 | 阿寶 | `d/snow/npc/child.c` | 盜賊 | 10 | C |
| 53 | 雪亭鎮 | 小孩 | `d/snow/npc/child1.c` | 平民 | 1 | C |
| 54 | 雪亭鎮 | 小孩 | `d/snow/npc/child2.c` | 平民 | 1 | C |
| 55 | 雪亭鎮 | 小孩 | `d/snow/npc/child3.c` | 平民 | 1 | C |
| 56 | 雪亭鎮 | 小孩 | `d/snow/npc/child4.c` | 平民 | 1 | C |
| 57 | 雪亭鎮 | 工頭 | `d/snow/npc/foreman.c` | 平民 | 8 | C |
| 58 | 雪亭鎮 | 瞎眼老太婆 | `d/snow/npc/gammer.c` | 盜賊 | 20 | A |
| 59 | 雪亭鎮 | 官兵 | `d/snow/npc/garrison.c` | 軍人 | 15 | B |
| 60 | 雪亭鎮 | 白衣女子 | `d/snow/npc/girl.c` | 武者 | 25 | S |
| 61 | 雪亭鎮 | 青衣漢子 | `d/snow/npc/guard.c` | 武者 | 10 | C |
| 62 | 雪亭鎮 | 藥鋪掌櫃 | `d/snow/npc/herbalist.c` | 平民 | 3 | C |
| 63 | 雪亭鎮 | （innkeeper） | `d/snow/npc/innkeeper.c` | 平民 | 4 | C |
| 64 | 雪亭鎮 | 拾荒老頭 | `d/snow/npc/junkman.c` | 平民 | 2 | C |
| 65 | 雪亭鎮 | 武官 | `d/snow/npc/lieutenant.c` | 軍人 | 24 | B |
| 66 | 雪亭鎮 | 楊大嬸 | `d/snow/npc/miller.c` | 平民 | 2 | C |
| 67 | 雪亭鎮 | 黑衣老人 | `d/snow/npc/oldman.c` | 平民 | 7 | B |
| 68 | 雪亭鎮 | 巡邏官兵 | `d/snow/npc/patrol.c` | 軍人 | 15 | B |
| 69 | 雪亭鎮 | 鐵匠 | `d/snow/npc/smith.c` | 平民 | 13 | C |
| 70 | 雪亭鎮 | 王懷芝 | `d/snow/npc/teacher.c` | 書生 | 5 | B |
| 71 | 雪亭鎮 | （waiter） | `d/snow/npc/waiter.c` | 平民 | 2 | C |
| 72 | 雪亭鎮 | 少婦 | `d/snow/npc/woman.c` | 平民 | 2 | C |
| 73 | 雪亭鎮 | 魚天 | `d/snow/npc/yu.c` | 平民 | 1 | C |
| 74 | 五堂鎮 | 冷梅莊二代弟子 | `d/wutang/npc/apprentice.c` | 武者 | 25 | B |
| 75 | 五堂鎮 | 大野豬 | `d/wutang/npc/big_boar.c` | 野獸 | — | 野獸強度 5 |
| 76 | 五堂鎮 | 野豬 | `d/wutang/npc/boar.c` | 野獸 | — | 野獸強度 4 |
| 77 | 五堂鎮 | 釣客 | `d/wutang/npc/fisher.c` | 平民 | 1 | C |
| 78 | 五堂鎮 | 護院武師 | `d/wutang/npc/guard.c` | 武者 | 5 | B |
| 79 | 五堂鎮 | 郭布 | `d/wutang/npc/guo_boo.c` | 道士 | 60 | S |
| 80 | 五堂鎮 | 小黃 | `d/wutang/npc/huang.c` | 平民 | 1 | C |
| 81 | 五堂鎮 | 呼延光 | `d/wutang/npc/huyen_guan.c` | 書生 | 35 | A |
| 82 | 五堂鎮 | 珍寶商人 | `d/wutang/npc/jeweller.c` | 平民 | 10 | C |
| 83 | 五堂鎮 | 廟祝 | `d/wutang/npc/keeper.c` | 平民 | 1 | C |
| 84 | 五堂鎮 | 李山 | `d/wutang/npc/lee_shan.c` | 平民 | 5 | C |
| 85 | 五堂鎮 | 陸四爺 | `d/wutang/npc/lu_estimator.c` | 平民 | 1 | C |
| 86 | 五堂鎮 | 老火 | `d/wutang/npc/old_fire.c` | 平民 | 1 | C |
| 87 | 五堂鎮 | 老人 | `d/wutang/npc/oldman.c` | 平民 | 1 | C |
| 88 | 五堂鎮 | 紅衣道士 | `d/wutang/npc/red_taoist.c` | 道士 | 15 | B |
| 89 | 五堂鎮 | 老駱 | `d/wutang/npc/ro.c` | 平民 | 1 | C |
| 90 | 五堂鎮 | 趙欽差 | `d/wutang/npc/royalist.c` | 軍人 | 1 | B |
| 91 | 五堂鎮 | 賣餅大叔 | `d/wutang/npc/seller.c` | 平民 | 1 | C |
| 92 | 五堂鎮 | 綿羊 | `d/wutang/npc/sheep.c` | 野獸 | — | 野獸強度 2 |
| 93 | 五堂鎮 | 牧羊人 | `d/wutang/npc/shepherd.c` | 平民 | 1 | C |
| 94 | 五堂鎮 | 煙波釣叟 | `d/wutang/npc/smoke_fisher.c` | 武者 | 40 | S |
| 95 | 五堂鎮 | 小桃 | `d/wutang/npc/tao.c` | 平民 | 5 | C |
| 96 | 五堂鎮 | 城隍爺 | `d/wutang/npc/town_god.c` | — | — | 不動（鬼魂） |
| 97 | 五堂鎮 | 店小二 | `d/wutang/npc/waiter.c` | 平民 | 1 | C |
| 98 | 五堂鎮 | 白衣道士 | `d/wutang/npc/white_taoist.c` | 道士 | 15 | B |
| 99 | 五堂鎮 | 葉翔 | `d/wutang/npc/yaesae.c` | 盜賊 | 30 | A |
| 100 | 五堂鎮 | 楊子陵 | `d/wutang/npc/yang_zlin.c` | 武者 | 45 | A |
| 101 | 五堂鎮 | 青年公子 | `d/wutang/npc/young_gentleman.c` | 平民 | 10 | C |
| 102 | 五堂鎮 | 小鼠兒 | `d/wutang/npc/young_man.c` | 平民 | 1 | C |
| 103 | 五堂鎮 | 青年書生 | `d/wutang/npc/young_scholar.c` | 平民 | 10 | C |
| 104 | 五堂鎮 | 雍泰 | `d/wutang/npc/yung_tai.c` | 武者 | 20 | B |
| 105 | 五堂鎮 | 梅影風 | `d/wutang/npc/may_yin_fong.c` | 武者 | 60 | S |
