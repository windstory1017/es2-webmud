// /d/lee/hunter.c
// 李家村復原版。GENERATED
// Topology follows the user-provided Li Village map; generated prose is intentionally replaceable.

inherit ROOM;

void create()
{
    set("short", "木屋");
    set("long", @LONG
屋內陳設雖然簡單，卻處處留著長年狩獵生活的痕跡。幾件簡單的家具散落其中。牆邊掛著幾件獵具，地上還堆著幾張尚未處理的獸皮。屋子中央擺著一張木桌。桌上除了幾件零散的獵具外，還放著一塊沾滿污漬的破布，似乎已經使用了很長一段時間。
LONG
    );
    set("objects", ([
        "/d/lee/npc/lee_yong" : 1
    ]));
    set("exits", ([
        "south" : __DIR__"wroad2"
    ]));
    set("detail/破布", "破布底下鼓鼓的，似乎藏了什麼東西，可以翻找 (search) 看看。");
    set("map/area", "李家村");
    setup();
    // MODIFIED: no replace_program(ROOM) — this file defines its own
    // init()/do_search(); replacing the program to plain ROOM after create()
    // would silently discard them, which is exactly why "search 破布"
    // wasn't working.
}

void init()
{
    ::init();
    add_action("do_search", "search");
}

int do_search(string arg)
{
    object dagger;

    if( !arg || arg != "破布" ) return 0;
    if( query("rag_searched") ) {
        write("你仔細翻找了那塊破布，但沒有再發現什麼。\n");
        return 1;
    }
    set("rag_searched", 1);
    message_vision("$N翻找了一下桌上的破布。\n", this_player());
    write("一把亮晃晃的匕首掉在地上。\n");
    dagger = new("/d/lee/obj/snow_dagger");
    dagger->move(this_object());
    return 1;
}
