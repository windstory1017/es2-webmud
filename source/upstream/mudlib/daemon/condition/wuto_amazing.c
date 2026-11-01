/* 驚異二針（amazing-needle）的霸甲／霸氣針力。NEW
 * 設計：docs/martial_arts/武陀灸堂_門派設計.md 第四章；共用邏輯見 wuto_effect.c。
 * data["kind"]：armor（驚異霸甲針）或 power（驚異霸氣針）。 */
inherit "/daemon/condition/wuto_effect";

void end_message(object me, mapping data)
{
    if( data["kind"] == "armor" )
        tell_object(me, "頭頂穴道的酸脹漸漸散去，周身那股堅凝之意也隨之消退了。\n");
    else
        tell_object(me, "頭頂穴道的熱意漸漸散去，四肢百骸那股霸道之氣也隨之消退了。\n");
}
