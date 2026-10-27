# 舊站 NPC 清單（custom/legacy_npc/）

> 依使用者提供的「舊有 NPC 待製作清單」（2026-10-15）製作，共 126 位，**尚未放入地圖**。
> 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（`docs/LEGACY_ROOM_NPC_COMPENDIUM.md`，DOCUMENTED）；
> 職業、等級、強度、門派、行為由使用者決定。屬性與精氣神由 NPC 強度系統計算（`docs/CUSTOM_NPC_POWER.md`），
> 技能由 NPC 自動技能系統依職業、等級、門派給（`docs/CUSTOM_NPC_SKILL.md`）。

## 製作規則

- 共用基底 `custom/legacy_npc/legacy_npc.c`：
  - 土匪型（`legacy_aggressive`）：看到玩家就下殺手。
  - 巡邏（`legacy_patrol`）：只在同一區域（房間的 map/area 相同）內走動，每 2 tick 擲一次，10% 機率移動。
- 道士且屬朱衣、素衣、玄衣派的，戰鬥中會施法（比照天師道各派弟子）。
- 門派名稱「武陀堂」「五陀灸堂」依使用者更正，統一為「武陀灸堂」。
- 使用者決定：司馬雷 ID 用 `Si-ma lei`；韓夫人 ID 用 `Madam han`；言歙（夜叉族不能當書生）改為平民；
  土伯（表上職業「野獸」）改為形天族武者；武官的大劍沿用現有 `obj/area/obj/bastardsword.c`。
- 新做的裝備：翠綠短杉 `custom/armor/cloth/green_cloth.c`、掩心甲 `custom/armor/armor/heart_armor.c`。
- 歐陽芝身上帶【天朝異事錄】，被殺時掉落。
- 頭銜與綽號從「完整稱呼」拆出；榮世國的稱呼只記到「榮府」，不設頭銜；任常恩有兩個頭銜，取第一個。

## 清單

| # | 名稱 | 英文 ID | 檔案 | 舊站區域／房間 | 種族 | 職業 | 等級 | 強度 | 門派 | 行為 |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 正閒道長 | Taoist master | `custom/legacy_npc/taoist_master.c` | 上清觀／賓客室側房 | 人類 | 道士 | 50 | B | — | — |
| 2 | 墮落的風神 | Evil spirit of storm | `custom/legacy_npc/evil_spirit_of_storm.c` | 京畿、平陽山、神域／後海湖、s路、迷霧中 | 人類族 | 道士 | 70 | S | 玄衣派（`taoist.storm`） | — |
| 3 | 考核官 | Checker | `custom/legacy_npc/checker.c` | 京畿／考選室 | 人類 | 軍人 | 20 | B | — | — |
| 4 | 武官 | Lieutenant | `custom/legacy_npc/lieutenant.c` | 兆隱縣、青邪宮／老王燒烤、上山小路 | 人類 | 軍人 | 25 | B | — | — |
| 5 | 公孫微 | Gong-sun wei | `custom/legacy_npc/gong_sun_wei.c` | 八寶樓／鬼境 | 焦僥族 | 盜賊 | 30 | B | — | 主動攻擊 |
| 6 | 司馬雷 | Si-ma lei | `custom/legacy_npc/si_ma_lei.c` | 八寶樓／— | 阿修羅族 | 軍人 | 35 | B | — | 主動攻擊 |
| 7 | 崇白虎 | Chong bai-hu | `custom/legacy_npc/chong_bai_hu.c` | 八寶樓／鑄刀房 | 夜叉族 | 武者 | 35 | B | 虎刀門（`fighter.tiger`） | 主動攻擊 |
| 8 | 白魚 | Bai yu | `custom/legacy_npc/bai_yu.c` | 八寶樓／魔境 | 巫首族 | 方士 | 40 | B | — | 主動攻擊 |
| 9 | 軒轅劍月 | Xun-yuan jian yue | `custom/legacy_npc/xun_yuan_jian_yue.c` | 八寶樓／頂樓 | 人類 | 武者 | 40 | A | 封山派（`fighter.fonxan`） | — |
| 10 | 馬鍾聖 | Ma zhong shen | `custom/legacy_npc/ma_zhong_shen.c` | 八寶樓／拳屋 | 黑齒族 | 武者 | 30 | B | — | 主動攻擊 |
| 11 | 冷吟月 | Leng yin yue | `custom/legacy_npc/leng_yin_yue.c` | 冷府／閨房、密室 | 人類 | 平民 | 1 | C | — | — |
| 12 | 應曉天 | Ying xiao tian | `custom/legacy_npc/ying_xiao_tian.c` | 冷梅莊／客房 | 人類 | 武者 | 40 | A | 封山派（`fighter.fonxan`） | — |
| 13 | 梅嵐武 | May lan-wu | `custom/legacy_npc/may_lan_wu.c` | 冷梅莊、文國縣／冷梅莊前院、清水鄉 | 人類 | 武者 | 30 | B | 冷梅莊（`fighter.lunmay`） | — |
| 14 | 景愈濤 | Jing yu tao | `custom/legacy_npc/jing_yu_tao.c` | 古劍門／書房 | 夜叉族 | 武者 | 70 | S | 古劍門（`fighter.ancient`） | — |
| 15 | 夜羅剎 | Yaksa | `custom/legacy_npc/yaksa.c` | 古墓場／— | 夜叉族 | 武者 | 30 | B | — | 巡邏 |
| 16 | 戰屍 | zombie warrior | `custom/legacy_npc/zombie_warrior.c` | 古墓場／— | 人類 | 軍人 | 30 | B | — | 主動攻擊、巡邏 |
| 17 | 羅剎王 | Yaksa King | `custom/legacy_npc/yaksa_king.c` | 古墓場／— | 夜叉族 | 武者 | 35 | A | — | 主動攻擊、巡邏 |
| 18 | 螟蛉子 | Ming ling zi | `custom/legacy_npc/ming_ling_zi.c` | 古墓場／內房 | 巫首族 | 道士 | 20 | A | — | — |
| 19 | 鬼夜叉 | Ghost yaksa | `custom/legacy_npc/ghost_yaksa.c` | 古墓場／— | 夜叉族 | 武者 | 30 | B | — | 主動攻擊、巡邏 |
| 20 | 歐陽芝 | O-yan zhi | `custom/legacy_npc/o_yan_zhi.c` | 名山書肆、八寶樓／歐陽書屋 | 人類 | 書生 | 30 | B | 步玄派（未列入對照表） | — |
| 21 | 劉乙忘玄 | Liuye the Master of Grimace | `custom/legacy_npc/liuye_the_master_of_grimace.c` | 哭笑門、青邪宮／青邪宮哭笑門鎮心 | 厭火族 | 武者 | 70 | S | 哭笑門（`fighter.chin`） | — |
| 22 | 劉可 | Liu ko | `custom/legacy_npc/liu_ko.c` | 哭笑門／— | 厭火族 | 平民 | 1 | C | — | — |
| 23 | 無慾師太 | Wu-yu nun | `custom/legacy_npc/wu_yu_nun.c` | 哭笑門／— | 人類 | 和尚 | 30 | B | — | — |
| 24 | 陰雨巖 | Yin euyen | `custom/legacy_npc/yin_euyen.c` | 喬陰縣／— | 人類 | 武者 | 20 | C | — | — |
| 25 | 韓文緒 | Han wen-suei | `custom/legacy_npc/han_wen_suei.c` | 喬陰縣／— | 厭火族 | 軍人 | 30 | B | — | — |
| 26 | 柳風 | Liu feng | `custom/legacy_npc/liu_feng.c` | 天寒村／『域水客棧』 | 巫首族 | 道士 | 20 | B | — | — |
| 27 | 桃仙 | Mountain god | `custom/legacy_npc/mountain_god.c` | 天山／桃花林、桃園 | 人類族 | 道士 | 30 | B | — | — |
| 28 | 安懶道人 | Old taoist | `custom/legacy_npc/old_taoist.c` | 天邪國／草廬 | 人類 | 道士 | 60 | A | 玄衣派（`taoist.storm`） | — |
| 29 | 於蘭兼毅 | Yu lan jian yi | `custom/legacy_npc/yu_lan_jian_yi.c` | 天邪國／— | 人類 | 武者 | 70 | S | 天邪派（未列入對照表） | — |
| 30 | 蕭朗生 | Hsiao lang-sheng | `custom/legacy_npc/hsiao_lang_sheng.c` | 天邪國／— | 人類 | 武者 | 50 | A | 天邪派（未列入對照表） | — |
| 31 | 白齊 | Taoist leader | `custom/legacy_npc/taoist_leader.c` | 太乙觀／賓客宿房 | 人類 | 道士 | 50 | A | 素衣派（`taoist.freeze`） | — |
| 32 | 孟驚鴻 | Meng jiang-hong | `custom/legacy_npc/meng_jiang_hong.c` | 孟家祖墳／— | 人類 | 武者 | 30 | B | — | — |
| 33 | 田由 | Tian yu | `custom/legacy_npc/tian_yu.c` | 孟家祖墳／— | 形天族 | 武者 | 30 | B | — | — |
| 34 | 花興 | Hua xing | `custom/legacy_npc/hua_xing.c` | 孟家祖墳／暗道 | 人類 | 盜賊 | 25 | B | — | — |
| 35 | 許彥之 | Hsu yen-chih | `custom/legacy_npc/hsu_yen_chih.c` | 季縣／— | 人類 | 武者 | 20 | B | 虎刀門（`fighter.tiger`） | — |
| 36 | 韓江洪 | Han jiang hong | `custom/legacy_npc/han_jiang_hong.c` | 尚書府／西廂房 | 人類 | 書生 | 20 | B | — | — |
| 37 | 柳淳風 | Liuh chuen-fon | `custom/legacy_npc/liuh_chuen_fon.c` | 康平村／小客店 | 人類 | 武者 | 30 | A | 封山派（`fighter.fonxan`） | — |
| 38 | 噬魂 | Ghost lord | `custom/legacy_npc/ghost_lord.c` | 文國縣／清水鄉 | 人類 | 軍人 | 35 | B | — | 主動攻擊 |
| 39 | 雷同 | Lay ton | `custom/legacy_npc/lay_ton.c` | 斐縣／— | 人類 | 方士 | 20 | B | 武陀灸堂（未列入對照表） | — |
| 40 | 任蕭蕭 | Ren xiao-xiao | `custom/legacy_npc/ren_xiao_xiao.c` | 未能判定／廂房 | 人類 | 平民 | 10 | B | — | — |
| 41 | 八臂石妖 | Stone ghost | `custom/legacy_npc/stone_ghost.c` | 未能判定／雷神宮殿 | 形天族 | 軍人 | 40 | A | — | 主動攻擊 |
| 42 | 兵部軍官 | Soldier leader | `custom/legacy_npc/soldier_leader.c` | 未能判定／— | 形天族 | 軍人 | 20 | B | — | — |
| 43 | 凌月蘅 | Ling yue heng | `custom/legacy_npc/ling_yue_heng.c` | 未能判定／花砌居 | 人類 | 武者 | 35 | A | 古劍派（`fighter.ancient`） | — |
| 44 | 劉尚民 | Liu shang min | `custom/legacy_npc/liu_shang_min.c` | 未能判定／— | 人類 | 武者 | 10 | C | — | — |
| 45 | 司馬成都 | Si-ma cheng du | `custom/legacy_npc/si_ma_cheng_du.c` | 未能判定／— | 人類 | 軍人 | 70 | A | — | — |
| 46 | 吳長貴 | Wu changuei | `custom/legacy_npc/wu_changuei.c` | 未能判定／— | 人類 | 盜賊 | 15 | B | 瑯夷派（`thief.longye`） | — |
| 47 | 地府鬼卒 | Hell soldier | `custom/legacy_npc/hell_soldier.c` | 未能判定／— | 人類 | 軍人 | 25 | B | — | — |
| 48 | 宇文嵐胥 | Yu wen | `custom/legacy_npc/yu_wen.c` | 未能判定／畫室內堂 | 人類 | 書生 | 20 | B | — | — |
| 49 | 左國益 | Zuo guo yi | `custom/legacy_npc/zuo_guo_yi.c` | 未能判定／— | 人類 | 軍人 | 30 | B | — | — |
| 50 | 李洪雲 | Lee hon_yun | `custom/legacy_npc/lee_hon_yun.c` | 未能判定／— | 夜叉族 | 武者 | 30 | B | 封山派（`fighter.fonxan`） | 主動攻擊 |
| 51 | 李瘋塵 | Lee fong chen | `custom/legacy_npc/lee_fong_chen.c` | 未能判定／— | 阿修羅族 | 武者 | 30 | B | — | 主動攻擊 |
| 52 | 東方孤冥 | Dong-fang gu ming | `custom/legacy_npc/dong_fang_gu_ming.c` | 未能判定／— | 人類 | 書生 | 65 | S | 玄天派（`scholar.sky`） | — |
| 53 | 林育崙 | Lin yu lun | `custom/legacy_npc/lin_yu_lun.c` | 未能判定／— | 人類 | 軍人 | 30 | A | — | — |
| 54 | 柳新雨 | Liuh xin yu | `custom/legacy_npc/liuh_xin_yu.c` | 未能判定／— | 人類 | 武者 | 30 | A | 封山派（`fighter.fonxan`） | — |
| 55 | 柳東蘆 | Liuh don-lu | `custom/legacy_npc/liuh_don_lu.c` | 未能判定／— | 人類 | 武者 | 70 | S | 封山派（`fighter.fonxan`） | — |
| 56 | 柳繪心 | Liuh hui xin | `custom/legacy_npc/liuh_hui_xin.c` | 未能判定／— | 人類 | 武者 | 20 | B | 封山派（`fighter.fonxan`） | — |
| 57 | 榮世國 | Rong shi guo | `custom/legacy_npc/rong_shi_guo.c` | 未能判定／— | 人類 | 武者 | 40 | B | — | — |
| 58 | 淩筠語 | Ling yun-yu | `custom/legacy_npc/ling_yun_yu.c` | 未能判定／— | 人類 | 平民 | 20 | B | — | — |
| 59 | 滅心 | Mie xin | `custom/legacy_npc/mie_xin.c` | 未能判定／— | 人類族 | 軍人 | 40 | A | — | 主動攻擊 |
| 60 | 滅慾 | Mie yu | `custom/legacy_npc/mie_yu.c` | 未能判定／— | 人類族 | 軍人 | 40 | A | — | 主動攻擊 |
| 61 | 王燕 | Wang yean | `custom/legacy_npc/wang_yean.c` | 未能判定／瓦屋 | 人類 | 方士 | 20 | B | — | — |
| 62 | 白無常 | White hell agent | `custom/legacy_npc/white_hell_agent.c` | 未能判定／— | 巫首族 | 道士 | 50 | A | 素衣派（`taoist.freeze`） | — |
| 63 | 秦朝天 | Chin chao tian | `custom/legacy_npc/chin_chao_tian.c` | 未能判定／— | 人類 | 武者 | 20 | B | — | — |
| 64 | 程槐 | Cheng huai | `custom/legacy_npc/cheng_huai.c` | 未能判定／— | 人類 | 武者 | 25 | B | — | — |
| 65 | 紅衣女子 | Red lady | `custom/legacy_npc/red_lady.c` | 未能判定／— | 人類 | 武者 | 40 | A | — | — |
| 66 | 肖麗穎 | Xiao li ying | `custom/legacy_npc/xiao_li_ying.c` | 未能判定／— | 人類 | 方士 | 25 | A | — | — |
| 67 | 蘇瀟 | Su xiao | `custom/legacy_npc/su_xiao.c` | 未能判定／書房 | 人類 | 武者 | 15 | A | — | — |
| 68 | 語小梅 | Yu shao me | `custom/legacy_npc/yu_shao_me.c` | 未能判定／— | 人類 | 盜賊 | 25 | B | — | — |
| 69 | 賈秋雨 | Jia qiu yu | `custom/legacy_npc/jia_qiu_yu.c` | 未能判定／— | 人類 | 武者 | 10 | C | — | — |
| 70 | 野人酋長 | Barbarian chief | `custom/legacy_npc/barbarian_chief.c` | 未能判定／— | 人類 | 武者 | 5 | C | — | 主動攻擊 |
| 71 | 鍾貙 | Zhong chu | `custom/legacy_npc/zhong_chu.c` | 未能判定／— | 黑齒族 | 軍人 | 40 | A | — | — |
| 72 | 青衣女郎 | Lady | `custom/legacy_npc/lady.c` | 未能判定／亂葬崗 | 人類 | 盜賊 | 5 | B | — | — |
| 73 | 韓夫人 | Madam han | `custom/legacy_npc/madam_han.c` | 未能判定／— | 夜叉族 | 平民 | 1 | C | — | — |
| 74 | 魯飛 | Lu fei | `custom/legacy_npc/lu_fei.c` | 未能判定／— | 厭火族 | 軍人 | 50 | A | — | — |
| 75 | 黃靖國 | Captain Huang | `custom/legacy_npc/captain_huang.c` | 未能判定／— | 人類 | 軍人 | 30 | A | — | — |
| 76 | 黑無常 | Black hell agent | `custom/legacy_npc/black_hell_agent.c` | 未能判定／— | 巫首族 | 道士 | 50 | A | 朱衣派（`taoist.fire`） | — |
| 77 | 李儒 | Li ru | `custom/legacy_npc/li_ru.c` | 柳家／民宅 | 人類 | 書生 | 20 | B | — | — |
| 78 | 劉開 | Liu kei | `custom/legacy_npc/liu_kei.c` | 檒城／街道 | 人類 | 軍人 | 30 | B | — | — |
| 79 | 駱易 | Ro yi | `custom/legacy_npc/ro_yi.c` | 步玄派／— | 人類 | 書生 | 65 | S | 步玄派（未列入對照表） | — |
| 80 | 武道通 | Wu dauton | `custom/legacy_npc/wu_dauton.c` | 武陀灸堂／武陀灸堂 | 人類 | 方士 | 60 | S | 武陀灸堂（未列入對照表） | — |
| 81 | 張小簽 | Zhang xiao qian | `custom/legacy_npc/zhang_xiao_qian.c` | 水嵐縣／街道 | 人類 | 平民 | 1 | C | — | — |
| 82 | 白劍飛 | Bai jian fei | `custom/legacy_npc/bai_jian_fei.c` | 漕幫／小船 | 夜叉族 | 武者 | 25 | B | 封山派（`fighter.fonxan`） | — |
| 83 | 蕭尹 | Xiao yin | `custom/legacy_npc/xiao_yin.c` | 漕幫／房間內 | 人類 | 武者 | 45 | A | — | — |
| 84 | 言歙 | Yan she | `custom/legacy_npc/yan_she.c` | 漕幫／縣衙客房 | 夜叉族 | 平民 | 20 | B | — | — |
| 85 | 艾小虎 | Ai xiao-hu | `custom/legacy_npc/ai_xiao_hu.c` | 焱硝山／山腳下 | 黑齒族 | 武者 | 45 | A | 虎刀門（`fighter.tiger`） | — |
| 86 | 衛兵 | Guard | `custom/legacy_npc/guard.c` | 白虎堂／白虎堂內院 | 夜叉族 | 軍人 | 20 | B | — | — |
| 87 | 韓龍 | Han long | `custom/legacy_npc/han_long.c` | 白虎堂／— | 夜叉族 | 武者 | 30 | B | — | — |
| 88 | 無念大師 | Abbot wu-nian | `custom/legacy_npc/abbot_wu_nian.c` | 白象寺／— | 人類 | 和尚 | 65 | A | 白象寺（未列入對照表） | — |
| 89 | 鍾豪 | Zhong hao | `custom/legacy_npc/zhong_hao.c` | 碧幽部落／鍾豪房 | 釘靈族 | 武者 | 10 | B | — | — |
| 90 | 趙弘春 | Zhao hong chun | `custom/legacy_npc/zhao_hong_chun.c` | 紫煙鎮／— | 人類 | 平民 | 5 | C | — | — |
| 91 | 上官小翠 | Senkun tez | `custom/legacy_npc/senkun_tez.c` | 老劍山／客房 | 人類 | 武者 | 10 | B | — | — |
| 92 | 晏訌 | Yen hong | `custom/legacy_npc/yen_hong.c` | 老劍山／客房 | 人類 | 武者 | 20 | B | — | — |
| 93 | 公孫志 | Gong-sun zhi | `custom/legacy_npc/gong_sun_zhi.c` | 茅山、丱天樹／紅燈客棧 | 人類 | 道士 | 20 | B | — | — |
| 94 | 土伯 | Guard of hell | `custom/legacy_npc/guard_of_hell.c` | 茅山／茅山石陣 | 形天族 | 武者 | 30 | B | — | — |
| 95 | 杜風凌 | Du feng ling | `custom/legacy_npc/du_feng_ling.c` | 茅山、李家村／小旅店 | 巫首族 | 道士 | 40 | A | — | — |
| 96 | 查思古禮 | Chaxigol | `custom/legacy_npc/chaxigol.c` | 茅山／大街 | 人類 | 武者 | 25 | B | — | — |
| 97 | 歐陽無寂 | Ou-yan wu ji | `custom/legacy_npc/ou_yan_wu_ji.c` | 茅山／客房內 | 人類 | 道士 | 30 | B | — | — |
| 98 | 歐陽無極 | Ou-yan wu ji | `custom/legacy_npc/ou_yan_wu_ji_2.c` | 茅山、康平村／小客店 | 人類 | 道士 | 40 | A | — | — |
| 99 | 趙曉九 | Zhao xiao jiu | `custom/legacy_npc/zhao_xiao_jiu.c` | 茅山、巫山／巫山客棧 | 人類 | 道士 | 20 | B | — | — |
| 100 | 孫漪 | Sun yi | `custom/legacy_npc/sun_yi.c` | 虎刀門／比武場 | 阿修羅族 | 武者 | 40 | A | 虎刀門（`fighter.tiger`） | — |
| 101 | 張雲 | Zhang yun | `custom/legacy_npc/zhang_yun.c` | 虎刀門／— | 人類 | 武者 | 35 | B | 虎刀門（`fighter.tiger`） | — |
| 102 | 田龍 | Tian long | `custom/legacy_npc/tian_long.c` | 虎刀門／客房 | 人類 | 武者 | 30 | B | 虎刀門（`fighter.tiger`） | — |
| 103 | 虎刀門教頭 | Tiger trainer | `custom/legacy_npc/tiger_trainer.c` | 虎刀門／操練場 | 人類 | 武者 | 20 | B | 虎刀門（`fighter.tiger`） | — |
| 104 | 謝青翔 | Xie qing-xiang | `custom/legacy_npc/xie_qing_xiang.c` | 虎刀門／客房內 | 夜叉族 | 武者 | 25 | B | 虎刀門（`fighter.tiger`） | — |
| 105 | 韓笑 | Han xiao | `custom/legacy_npc/han_xiao.c` | 虎刀門／— | 人類 | 武者 | 60 | S | 虎刀門（`fighter.tiger`） | — |
| 106 | 刑龍 | Xing long | `custom/legacy_npc/xing_long.c` | 銅佛寺／客房 | 形天族 | 武者 | 35 | A | 哭笑門（`fighter.chin`） | — |
| 107 | 魯驚 | Lu jing | `custom/legacy_npc/lu_jing.c` | 銅佛寺／客房 | 人類 | 軍人 | 25 | B | — | — |
| 108 | 邯逸山 | Island master | `custom/legacy_npc/island_master.c` | 陷空島／賓客室側房 | 人類 | 武者 | 40 | B | — | — |
| 109 | 利丰華 | Lee fong-hwa | `custom/legacy_npc/lee_fong_hwa.c` | 隱教／練武場 | 阿修羅族 | 盜賊 | 50 | A | 隱教（`thief.hidden`） | — |
| 110 | 黃十四娘 | Huang | `custom/legacy_npc/huang.c` | 隱教／山洞峭壁前 | 人類 | 盜賊 | 40 | B | 隱教（`thief.hidden`） | — |
| 111 | 佛心 | Phoxin | `custom/legacy_npc/phoxin.c` | 雪吟莊／天井 | 人類 | 盜賊 | 50 | S | — | — |
| 112 | 哭無望 | Ku wu wang | `custom/legacy_npc/ku_wu_wang.c` | 雪吟莊／東廂房 | 厭火族 | 武者 | 40 | A | 哭笑門（`fighter.chin`） | — |
| 113 | 褚商魂 | Chu xang-when | `custom/legacy_npc/chu_xang_when.c` | 雪吟莊／雪吟莊大廳 | 人類 | 武者 | 25 | B | 雪吟莊（`fighter.snow`） | — |
| 114 | 魚平胤 | Yu pingyin | `custom/legacy_npc/yu_pingyin.c` | 雪吟莊／後花園 | 人類 | 武者 | 20 | B | 雪吟莊（`fighter.snow`） | — |
| 115 | 魚鐵山 | Yu tieshan | `custom/legacy_npc/yu_tieshan.c` | 雪吟莊／雪吟莊大廳 | 人類 | 武者 | 65 | S | 雪吟莊（`fighter.snow`） | — |
| 116 | 黑衣人 | Man in black | `custom/legacy_npc/man_in_black.c` | 雲棧市集／客房 | 厭火族 | 盜賊 | 20 | B | — | — |
| 117 | 任常恩 | Jen commando | `custom/legacy_npc/jen_commando.c` | 青邪宮／上山小路 | 人類 | 軍人 | 25 | B | — | — |
| 118 | 葬心 | Blade man | `custom/legacy_npc/blade_man.c` | 青邪宮、漕幫／酒館二樓 | 厭火族 | 武者 | 30 | A | 哭笑門（`fighter.chin`） | — |
| 119 | 司馬蔚 | Si-ma wei | `custom/legacy_npc/si_ma_wei.c` | 風城／— | 人類 | 軍人 | 55 | A | — | — |
| 120 | 張俊仁 | Zhang jun ren | `custom/legacy_npc/zhang_jun_ren.c` | 鬼洞／— | 人類 | 武者 | 40 | B | 封山派（`fighter.fonxan`） | — |
| 121 | 徐小惡 | Xu xiao e | `custom/legacy_npc/xu_xiao_e.c` | 黃家鏢局／— | 人類 | 盜賊 | 25 | B | （門派以後再補） | — |
| 122 | 高風 | Kao fon | `custom/legacy_npc/kao_fon.c` | 黃家鏢局／練功房 | 形天族 | 武者 | 20 | B | — | — |
| 123 | 黃天霸 | Huang tien-ba | `custom/legacy_npc/huang_tien_ba.c` | 黃家鏢局、茅山、文國縣／外廳、山林、清水鄉 | 夜叉族 | 武者 | 30 | A | 虎刀門（`fighter.tiger`） | — |
| 124 | 張霸 | Chang ba | `custom/legacy_npc/chang_ba.c` | 黑風寨／山谷中 | 人類 | 武者 | 50 | A | 虎刀門（`fighter.tiger`） | — |
| 125 | 蕭秋山 | Shao chu-san | `custom/legacy_npc/shao_chu_san.c` | 黑風寨／武器室 | 夜叉族 | 武者 | 30 | B | — | 主動攻擊 |
| 126 | 閰狂風 | Yan kuang phon | `custom/legacy_npc/yan_kuang_phon.c` | 黑風寨／黑風寨大廳 | 夜叉族 | 武者 | 40 | B | 封山派（`fighter.fonxan`） | 主動攻擊 |

## 尚未製作的裝備（NPC 身上先空著）

共 34 種。做好後在對應 NPC 檔案加上 `carry_object(...)` 即可。

| 裝備 | 誰身上 |
|---|---|
| 【脈穴內經】(Book of anatomy) | 桃仙 |
| 五行針(Needle of alchemy) | 葬心 |
| 僧衣(Robe) | 無慾師太 |
| 大斗笠(Big cap) | 公孫志、徐小惡 |
| 大蠻刀(Heavy blade) | 查思古禮 |
| 天山野蔘鬚(Senxu) | 桃仙 |
| 太陽大鵬巨神劍(Sun colosus sword of eagle) | 張俊仁 |
| 太陽巨神劍(Sun colosus sword) | 羅剎王、司馬蔚 |
| 寬襟長衫(Long robe) | 查思古禮 |
| 戰矛(Ｗar lance) | 戰屍 |
| 牛皮短靴(Leather boots) | 魚平胤 |
| 獸皮裙(Fur sarong) | 野人酋長 |
| 琥珀心鍊(Amber necklace) | 任蕭蕭 |
| 皮甲(Leather armor) | 劉開 |
| 短獵刀(Tanto) | 紅衣女子 |
| 粗麻布衣(Cloth) | 程槐 |
| 精鋼短刀(Steel kris) | 徐小惡 |
| 素雲頭巾(Cloud-circlet) | 東方孤冥 |
| 絲綢女衫(Silk dress) | 柳繪心 |
| 纏布刀(Wrapped blade) | 蕭尹 |
| 羊皮短靴(Goatskin buskins) | 青衣女郎 |
| 虎紋斬馬刀(Reckless blade) | 韓笑、張霸 |
| 虹綢帶(Rainbow belt) | 語小梅 |
| 襯甲(Padded armor) | 羅剎王、吳長貴 |
| 軟綢長袍(Silk robe) | 賈秋雨、魚平胤 |
| 逐風刃(Windfury sword) | 駱易 |
| 錦袍(Expensive cloth) | 柳風、李儒 |
| 鐵鱗腿護(Iron scale legs) | 兵部軍官、司馬成都 |
| 鑌鐵頭盔(Iron helm) | 左國益 |
| 長袍馬掛(Robe) | 趙弘春 |
| 青布小襖(Green jerkin) | 青衣女郎 |
| 青衣(Green cloth) | 黃靖國 |
| 青鐵棍筆(Iron pen) | 宇文嵐胥 |
| 點鋼匕(Steel dagger) | 吳長貴 |

## 尚未列入門派武功對照表的門派

這些 NPC 先不設門派記號，只有依職業給的技能。

| 門派 | NPC |
|---|---|
| 步玄派 | 歐陽芝、駱易 |
| 天邪派 | 於蘭兼毅、蕭朗生 |
| 武陀灸堂 | 雷同、武道通 |
| 白象寺 | 無念大師 |
