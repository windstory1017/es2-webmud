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

## 測試中的新基底 /inherit/dummy.c（2026-11-19）

使用者實測發現：偷襲現有假人時，在「繞到背後」那一步被察覺，假人會出手打玩家一下
（「你被假人攻了個措手不及！」）。原因是偷襲指令被察覺時會直接叫目標出手，
而 `/feature/npc/dummy.c` 只擋了 `fight_ob()`、`kill_ob()`，沒有擋 `attack()`。

新基底 `/inherit/dummy.c` 以 `/feature/npc/dummy.c` 為底本另寫一份（不繼承它），另外補上：

| 項目 | 做法 | 效果 |
|---|---|---|
| 完全不出手 | `attack()` 不做任何事 | 任何地方叫假人攻擊都不會動作，修好上面的還手漏洞 |
| 沒有警覺、防禦、應變 | `query_ability()` 的 awarness、defense、wittiness 一律為 0 | 偷襲兩次察覺判定一定不被發現；偷襲得手後的追擊只看玩家有沒有殺手本能；戰鬥中玩家每一拍都會對它出手 |
| 精為 0 | `query_stat("gin")` 為 0 | 偷襲察覺判定裡的隨機精不加分；假人不會昏倒，不受影響 |

其餘照舊：不還手、必定命中（戰鬥系統看 `training_dummy`）、不損壞、沒有生命現象、
毒與狀態上不了身；喚蟒之術、茅山陰魂附身照舊不能對假人施放。

測試用假人：石製假人 `custom/wizroom/w/windstory/obj/stone_stake.c`
（ID：stone stake，繼承 `/inherit/dummy`）。

暫不做（2026-11-19 使用者決定先取消）：耐久度、擊中扣耐久、對假人練功的技能上限。
已知現象：偷襲得手後追擊成功時會打三下；偷襲假人照樣增加罪孽。

測試沒問題後，再把這些內容併回現有假人的繼承檔 `/feature/npc/dummy.c`。
