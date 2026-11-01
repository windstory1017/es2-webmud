/* 引源針法（potency-needle）的強化針力。NEW
 * 設計：docs/martial_arts/武陀灸堂_門派設計.md 第六章；共用邏輯見 wuto_effect.c。
 * data["kind"]：awareness、move 或 element。 */
inherit "/daemon/condition/wuto_effect";

void end_message(object me, mapping data)
{
    switch(data["kind"]) {
    case "awareness":
        tell_object(me, gender_self(me) + "耳目間那份清明漸漸淡去，四周的動靜又模糊了起來。\n");
        break;
    case "move":
        tell_object(me, gender_self(me) + "腿上經絡的暖意漸漸退去，腳下又沉了幾分。\n");
        break;
    default:
        tell_object(me, gender_self(me) + "掌心那股躁動的元氣漸漸平息下來。\n");
    }
}
