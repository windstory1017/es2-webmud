// Longtu HEAT B, isolated implementation. UTF-8 LPC, not C.
#define HB "/daemon/misc/"
#define HEAT_D HB "heat_d"
#define HEAT_ITEM "/obj/alchemist/" // 丹爐、丹鼎、松柴、焦灰（2026-10-15 使用者決定）
#define MIXTURE_DIR "/obj/medication/alchemist/" // 藥方與成品同放 alchemist（2026-10-15 使用者決定）
#define MIXTURE_WEIGHT 130 // 配方每份重量（2026-10-15 使用者決定 R9；大還丹除外）
#define MAJOR_MIXTURE_WEIGHT 8000 // 大還丹配方每份重量（2026-10-15 使用者決定 R4）
#define AROMA "一陣陣奇異的藥香自丹爐中緩緩飄散，初聞帶著些許草木的清苦，細嗅之下卻又透出一絲溫潤甘甜。\n"
#define CHARRED "丹爐中不斷散發出陣陣惡臭，焦糊的藥渣混雜著嗆人的煙火氣息，濁氣久久不散，令人難以靠近。\n"
#define MIXED "諸般藥材相互摻合，或粗或細，或深或淺，層層交錯，漸漸混作一團，再難辨其原本模樣。\n"
#define UNKNOWN_ASH "爐火一經催動，混雜的藥料便迅速焦黑蜷縮，原本斑駁的色澤轉瞬被焦痕吞沒，伴隨著一陣刺鼻的焦臭，頃刻間化作一堆烏黑的焦灰。\n"
#define SPLIT "自數種不同的藥材提出原料，但色澤各異的藥末與碎屑已交錯混合，使其成為你難以分辨原貌的混合藥材。\n"
#define WATER_OUT "燃燒中的松柴在浸入水中的同時，發出陣陣滋滋聲響，火光迅速黯淡，轉眼間便徹底熄滅。\n"
