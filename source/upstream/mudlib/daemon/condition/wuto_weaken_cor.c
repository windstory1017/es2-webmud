/* 五敗針法（wuto-needle）暫時削減的膽識。NEW
 * 設計：docs/martial_arts/武陀灸堂_門派設計.md 第五章；共用邏輯見 wuto_effect.c。 */
inherit "/daemon/condition/wuto_effect";

void end_message(object me, mapping data)
{
    tell_object(me, gender_self(me) + "胸中的膽氣慢慢回來了。\n");
}
