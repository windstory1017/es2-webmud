# 練武假人（無生命體 NPC）

## 問題
假人原本是完整的活人角色（繼承 `F_VILLAGER`），所以會有心跳、狀態值、中毒，
也會昏倒再甦醒。各處補的 `training_dummy` 檢查只擋得住有人想到的路徑。

## 做法
- 共用基底 `/feature/npc/dummy.c`：不還手、不閃避、不損壞；不會昏倒、甦醒、死亡；
  不耗精氣神、體力、飲食，不疲勞、不回血；心跳關閉（被重新打開時下一拍再關）。
  挨打仍回傳正的傷害值，修練經驗照常累計。
- 毒與狀態：`/feature/char/condition.c` 的 `set_condition()` 遇到 `training_dummy` 直接略過，
  所以任何毒、法術狀態都上不了假人。
- 假人類 NPC 一律 `inherit "/feature/npc/dummy";`，只寫名字與說明，不再各自複製覆寫。

## 目前的假人
| NPC | 檔案 |
|---|---|
| 振武軍營假人 | `custom/zhenwu/npc/target_stake.c` |
| 冷梅莊假人 | `d/swaylan/lunmay/npc/stake.c` |
| 百花村石獅子 | `d/baihua/npc/stone_lion.c` |

全專案搜過 `training_dummy`、木樁、石獅、石像、稻草人等，只有這三個是假人；
八臂石妖（`custom/legacy_npc/stone_ghost.c`）是會攻擊的怪物，不屬於假人。
新增假人請繼承基底；測試 `tests/test_v32060_training_dummy.py` 會檢查所有設了 `training_dummy` 的 NPC 都走基底。
