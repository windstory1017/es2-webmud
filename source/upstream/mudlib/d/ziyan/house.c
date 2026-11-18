// /d/ziyan/house.c — 紫煙鎮（M10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_search", "search");
}

/* 搜屋：先看到書櫃旁的血手印，搜書櫃找到血書（每次房間重置只有一封）。
 * 玄冰烈焰斧任務（docs/martial_arts/玄冰烈焰斧_任務設計.md）：另有三處可搜的線索（地板、封條、牆縫），
 * 每位玩家各發現一次，記在玩家身上 quest/ziyan_axe_clue（1 地板、2 封條、4 牆縫、8 拿過血書）；
 * 四項齊備時案情大白，quest/ziyan_axe 設為 2。 */
#define QUEST  "quest/ziyan_axe"
#define CLUES  "quest/ziyan_axe_clue"

private void check_story(object me)
{
    if( (int)me->query(CLUES) < 15 || (int)me->query(QUEST) >= 2 ) return;
    me->set(QUEST, 2);
    tell_object(me, "你把鏢單、告示與黑羽箭擺在一起，心裡漸漸有了輪廓：魏長青押的鏢被山賊劫了，"
        "縣令趙弘春與賊人同流合污，事後為了滅口，反誣魏長青私吞鏢銀，連他全家都不放過。"
        "這封血書，想必是要交給某個人的。\n");
}

private int find_clue(object me, int bit, string msg)
{
    if( (int)me->query(CLUES) & bit ) {
        write("你把這裡又翻了一遍，再也找不到什麼。\n");
        return 1;
    }
    me->set(CLUES, (int)me->query(CLUES) | bit);
    write(msg);
    if( !(int)me->query(QUEST) ) me->set(QUEST, 1);
    check_story(me);
    return 1;
}

int do_search(string arg)
{
    object me = this_player(), letter;

    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( !arg ) {
        write("你仔細觀察屋內，發現書櫃側邊有個不起眼的血手印，看起來似乎不太一般。牆角的縫隙裡似乎還卡著什麼東西。\n");
        return 1;
    }
    if( arg == "地板" || arg == "石板縫" || arg == "石板" )
        return find_clue(me, 1, "你蹲下身仔細摸索，從石板縫裡抽出半張被血泡得發硬的紙，上面依稀是景隆錢莊的鏢單，"
            "押鏢人一欄寫著魏長青，鏢銀的去向卻被血漬糊成了一團。\n");
    if( arg == "封條" || arg == "告示" )
        return find_clue(me, 2, "你湊近細看門板上殘存的封條與告示，上頭寫著魏長青監守自盜、私通山賊，"
            "落款蓋著縣衙的大印，簽押的正是縣令趙弘春。\n");
    if( arg == "牆縫" || arg == "牆角" || arg == "縫隙" )
        return find_clue(me, 4, "你在牆縫裡摸出半截黑羽箭，箭桿粗糙，絕不是官府的制式，倒像是山賊慣用的東西。\n");
    if( arg != "書櫃" ) return 0;
    if( query_temp("letter_taken") ) {
        write("你把書櫃翻了個遍，再也找不到什麼。\n");
        return 1;
    }
    set_temp("letter_taken", 1);
    message_vision("$N在書櫃中發現一封血書。\n", me);
    letter = new("/d/ziyan/obj/bloody_letter");
    if( !letter->move(me) ) letter->move(this_object());
    me->set(CLUES, (int)me->query(CLUES) | 8);
    if( !(int)me->query(QUEST) ) me->set(QUEST, 1);
    check_story(me);
    return 1;
}

void reset()
{
    ::reset();
    delete_temp("letter_taken");
}

void create()
{
    set("short", "民宅");
    set("long", @LONG
屋內一片狼藉，桌椅東倒西歪，地上的血跡雖已被清洗過，石板縫裡仍留著洗不淨的暗紅。這裡是近日發生兇案的現場，官府草草收拾後便貼上封條了事，倒是靠牆的書櫃還立得端端正正。
LONG
    );
    set("detail", ([
        "血手印" : "書櫃旁這道倉促留下的手印，明顯有古怪。",
    ]));
    set("detail/地板", "石板縫裡的暗紅怎麼也洗不乾淨，縫隙深處似乎還塞著什麼，可以仔細摸索 (search) 看看。");
    set("detail/石板縫", "石板縫裡的暗紅怎麼也洗不乾淨，縫隙深處似乎還塞著什麼，可以仔細摸索 (search) 看看。");
    set("detail/封條", "官府貼在門板上的封條與告示已被風吹得殘破，字跡還依稀可辨，可以湊近細看 (search)。");
    set("detail/牆縫", "牆角的縫隙裡似乎卡著什麼東西，可以伸手摸摸 (search) 看。");
    set("detail/書櫃", "靠牆的書櫃立得端端正正，櫃中書冊塞得滿滿的，或許可以翻找 (search) 看看。");
    set("exits", ([
        "east" : __DIR__"lane",
    ]));
    set("map/area", "水嵐縣");
    setup();
}
