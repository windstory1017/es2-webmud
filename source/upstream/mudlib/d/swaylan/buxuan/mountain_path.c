// /d/swaylan/buxuan/mountain_path.c — 落雲峰（AI16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山間小路");
    set("long", @LONG
山路漸漸陡起來，兩旁的松林愈發茂密，林間飄著淡淡的雲氣，終年不散。路面鋪著碎石，被往來的腳步踩得平實，偶爾能看見挑著擔子的人影，在雲霧裡時隱時現，轉眼又不見了。
LONG
    );
    set("exits", ([
        "southwest" : __DIR__"slope",
        "northeast" : __DIR__"gate",
    ]));
    set("map/area", "落雲峰");
    setup();
    replace_program(ROOM);
}
