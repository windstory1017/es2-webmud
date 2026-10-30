// /d/swaylan/pawnshop.c — 水嵐縣（Y18）。房間敘述照舊站原文。

#include <room.h>

inherit HOCKSHOP;

void create()
{
    set("short", "高陽當鋪");
    set("long", @LONG
在你面前的是一間當舖，你可以在這裡將你不想要的東西賣掉，當然你也可以在這裡買到一些好的裝備或武器，不過通常要付出的價錢會比平常多。在這裡買賣，絕對是童叟無欺，不會因為你是惡霸或是土匪比較便宜，也不會因為你是小孩子而賣你特別貴。所以高陽當舖在水嵐縣就漸漸的出名了。
LONG
    );
    set("exits", ([
        "south" : __DIR__"street2",
    ]));
    set("objects", ([
        __DIR__"npc/pawn_guard" : 1,
    ]));
    set("no_fight", 1);
    set("map/area", "水嵐縣");
    setup();
}
