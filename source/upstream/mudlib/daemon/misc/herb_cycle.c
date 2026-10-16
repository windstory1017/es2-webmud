/*---
description: 龍圖 HERB 成品購買週期（2026-10-14 決議）。
rule: 以伺服器開機後經過秒數 uptime() 每 15 分鐘切一格。玩家本週期第一次購買
      成品時，依當下所在格開啟週期，格起點加 30 分鐘重置。八種丹藥共用週期、
      數量各自計算。重開機即重置；重載 LPC、登出再登入不重置。
      不使用計時器、reset() 或離線掃描。
---*/

#define SLOT 900          // 15 分鐘
#define SPAN 1800         // 格起點加 30 分鐘
#define BOOT_TOLERANCE 5  // 開機時間點容許誤差（秒）

void create() { seteuid(getuid()); }

int query_boot() { return time() - uptime(); }

private int same_boot(int recorded)
{
    int d;
    d = recorded - query_boot();
    if (d < 0) d = -d;
    return d <= BOOT_TOLERANCE;
}

// mark 格式：([ "boot": int, "end": int, "counts": mapping ])
// 回傳：1 有效週期、0 無紀錄或已失效、-1 格式異常。
int valid(mixed mark)
{
    if (!mark) return 0;
    if (!mapp(mark) || !intp(mark["boot"]) || !intp(mark["end"]) || !mapp(mark["counts"]))
        return -1;
    if (!same_boot(mark["boot"])) return 0;
    if (uptime() >= mark["end"]) return 0;
    return 1;
}

// 本週期某丹已購數量（mark 無效時為 0）。
int used(mixed mark, string key)
{
    mixed n;
    if (valid(mark) != 1) return 0;
    n = mark["counts"][key];
    return intp(n) && n > 0 ? n : 0;
}

// 成功購買後的新 mark（不直接寫入玩家；由交易核心寫入與回復）。
mapping next_mark(mixed mark, string key, int count)
{
    mapping counts;
    int start, boot, end;
    if (valid(mark) == 1) {
        boot = mark["boot"];
        end = mark["end"];
        counts = copy(mark["counts"]);
    } else {
        boot = query_boot();
        start = uptime() - uptime() % SLOT;
        end = start + SPAN;
        counts = ([]);
    }
    if (!intp(counts[key]) || counts[key] < 0) counts[key] = 0;
    counts[key] += count;
    return ([ "boot": boot, "end": end, "counts": counts ]);
}
