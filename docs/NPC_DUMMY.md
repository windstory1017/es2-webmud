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
（ID：stone stake、stake、stone，繼承 `/inherit/dummy`）。

暫不做（2026-11-19 使用者決定先取消）：耐久度、擊中扣耐久、對假人練功的技能上限。
已知現象：偷襲得手後追擊成功時會打三下；偷襲假人照樣增加罪孽。

測試沒問題後，再把這些內容併回現有假人的繼承檔 `/feature/npc/dummy.c`。

## 自有流程的技能打假人（2026-11-19）

有些技能不走一般戰鬥（`COMBAT_D->fight()`），自己寫命中敘述、自己報體力，
所以戰鬥系統對假人的特別處理管不到。使用者實測五敗針法打石製假人時，
出現「冷汗涔涔」「身子一僵」和「( 石製假人看起來對這種程度的攻擊一點也不在乎。 )」。

處理原則（使用者定案）：
- 死物沒有體力：`/inherit/dummy.c` 把氣的上限固定為 0，`report_status()` 與各技能報體力前都會先看上限，
  是 0 就不報。（新版武功共用引擎的 `show_status()` 本來就跳過假人。）
- 中招敘述只寫外觀留下的痕跡，不寫痛、抖、麻、昏這類活人反應；木頭、石頭都適用。
- 會讓目標停頓的效果，打假人時不觸發，也不顯示停頓的敘述。
- 法術打假人必定命中（天師道法、少陰咒法共用 `taoism-spells.c` 的 `spell_hits()`）。
- 吸取、回復類效果（地火回流、兆冰、七重玄冰、天劍三招破勢）打假人時**照舊**，不取消。
- 施展者自己的條件決定的成敗（例如天劍的困天訣、三招破勢）照舊會失敗，只把失敗敘述換成不寫對方反應的版本。

| 技能 | 檔案 | 打假人時 |
|---|---|---|
| 五敗針法 | `daemon/skill/wuto-needle.c` | 五句中招敘述換成死物版本；不觸發停頓 |
| 天師道法 | `daemon/skill/taoism-spells.c` | 四系十六句命中敘述換成死物版本；三昧真火延後爆發與預告換句、不停頓；無相冰不停頓；必定命中 |
| 少陰咒法 | `daemon/skill/taoism_of_darkness.c` | 鬼火、冥火命中敘述換成死物版本（命中照天師道法，必中） |
| 傲梅暗劍訣 | `daemon/skill/advance_lunmay.c` | 冰凍換句、不停頓 |
| 天邪掌 | `daemon/skill/celestial_palm.c` | 定身不觸發 |
| 天邪內功 | `daemon/skill/celestial_force.c` | 天邪虎嘯換句、不停頓 |
| 五虎斷門刀 | `daemon/skill/five-tiger_blade.c` | 五虎斷門換句、不停頓 |
| 天劍（進階） | `daemon/skill/advance-sky_sword.c` | 困天訣成功、失敗換句、不停頓；三招破勢成功、失敗換句 |
| 引源針法 | `daemon/skill/potency-needle.c` | 可以對假人施針（照樣有經驗），四種效果都換成「針尾在$n身上輕輕晃了晃，$n毫無反應。」 |

這是用關鍵字掃出來的清單，可能有漏掉的；實測再發現怪敘述就補進來。
