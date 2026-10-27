/*---
description: Mudlib simul efuns
---*/

#include "/adm/simul_efun/atoi.c"
#include "/adm/simul_efun/chinese.c"
#include "/adm/simul_efun/cursor.c"
#include "/adm/simul_efun/file.c"
#include "/adm/simul_efun/gender.c"
#include "/adm/simul_efun/object.c"
#include "/adm/simul_efun/path.c"
#include "/adm/simul_efun/realip.c"
#include "/adm/simul_efun/utf8.c"
#include "/adm/simul_efun/wizard.c"

// This must be after gender.c
#include "/adm/simul_efun/message.c"

// 用到 message.c 的 tell_object()，放在它後面。
#include "/adm/simul_efun/webhud.c"
