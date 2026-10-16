#include "heat_b.h"
// v2.1.0：成品改由 /obj/medication/alchemist/ 提供（2026-10-15 使用者決定）。
#define PRODUCT_DIR "/obj/medication/alchemist/"
private mapping rows;
void create() { rows = ([
"black": ({"烏心丹","black pill",80,250,100,150,({({8,25,1})}),PRODUCT_DIR "black_pill"}),
"refresh": ({"金鎖固精丸","refresh pill",110,160,400,479,({({11,13,2}),({14,16,4})}),PRODUCT_DIR "refresh_pill"}),
"mana": ({"醒神丹","mana pill",130,150,450,491,({({13,13,1}),({14,14,2}),({15,15,4})}),PRODUCT_DIR "mana_pill"}),
"minor": ({"小還丹","minor heal pill",130,150,533,575,({({13,13,1}),({14,14,2}),({15,15,4})}),PRODUCT_DIR "minor_heal_pill"}),
"major": ({"大還丹","major heal pill",460,500,1900,2100,({({46,47,1}),({48,49,2}),({50,50,4})}),PRODUCT_DIR "major_heal_pill"}),
"aqua": ({"金液丹","aquapill",90,180,420,540,({({9,12,1}),({13,16,2}),({17,18,4})}),PRODUCT_DIR "aquapill"}),
"green": ({"甘靈清毒丹","green pill",90,230,420,959,({({9,14,1}),({15,19,2}),({20,23,4})}),PRODUCT_DIR "green_pill"}),
"lavage": ({"清腹丹","lavage pill",130,150,200,400,({({13,14,2}),({15,15,4})}),PRODUCT_DIR "lavage_pill"}),
"relax": ({"養心丹","relax pill",110,160,400,479,({({11,13,2}),({14,16,4})}),PRODUCT_DIR "relax_pill"})
]); }
mapping query_recipes() { return rows; }
string mixture_path(string key) { return MIXTURE_DIR "mixture_" + key + (key=="unknown" ? "" : "pill"); }
string product_path(string key) { return query_recipes()[key][7]; }
int increment(string key, int wood) {
    mixed *row,*band;
    row=query_recipes()[key]; if(!arrayp(row)) return 0;
    foreach(band in row[6]) if(wood>=band[0] && wood<=band[1]) return (wood-row[2]/10+1)*band[2];
    return 0;
}
