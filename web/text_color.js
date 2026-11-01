// 設定 → 文字顏色：訊息欄與聊天視窗的基礎文字顏色（遊戲沒有指定顏色的文字，以及遊戲指定的「白色」）。
// 八個預設色（白、綠各四階，不含最亮的），或用紅綠藍三條與明暗一條自己調。設定記在瀏覽器。

const STORAGE_KEY='es2-ui-text-color-v1';
const DEFAULT_RGB=[0xe3,0xec,0xe5];
export const TEXT_COLOR_PRESETS=[
  {name:'白一',hex:'#c8c4bb'},{name:'白二',hex:'#b0aca3'},{name:'白三',hex:'#98948c'},{name:'白四',hex:'#807d76'},
  {name:'綠一',hex:'#a3c4ab'},{name:'綠二',hex:'#8bab93'},{name:'綠三',hex:'#74927c'},{name:'綠四',hex:'#5e7a66'},
];

const hexToRgb=h=>[1,3,5].map(i=>parseInt(h.slice(i,i+2),16));
const rgbToHex=c=>'#'+c.map(v=>Math.round(v).toString(16).padStart(2,'0')).join('');
const clamp=(v,lo,hi)=>Math.min(hi,Math.max(lo,Number(v)||0));

export function setupTextColor(root){
  if(!root)return;
  const swatches=root.querySelector('[data-text-swatches]');
  const sliders={r:root.querySelector('[data-text-rgb="r"]'),g:root.querySelector('[data-text-rgb="g"]'),b:root.querySelector('[data-text-rgb="b"]')};
  const bright=root.querySelector('[data-text-bright]');
  const preview=root.querySelector('[data-text-preview]');
  let rgb=[...DEFAULT_RGB],level=100;

  try{
    const saved=JSON.parse(localStorage.getItem(STORAGE_KEY)||'null');
    if(saved&&Array.isArray(saved.rgb)&&saved.rgb.length===3){rgb=saved.rgb.map(v=>clamp(v,0,255));level=clamp(saved.level??100,20,100);}
  }catch(_){}

  const finalHex=()=>rgbToHex(rgb.map(v=>v*level/100));
  function apply(save=true){
    const hex=finalHex();
    document.documentElement.style.setProperty('--term-fg',hex);
    ['r','g','b'].forEach((k,i)=>{
      const s=sliders[k];if(!s)return;
      s.value=rgb[i];
      const out=root.querySelector(`[data-text-rgb-value="${k}"]`);if(out)out.textContent=rgb[i];
    });
    if(bright){bright.value=level;const out=root.querySelector('[data-text-bright-value]');if(out)out.textContent=level+'%';}
    if(preview){preview.style.color=hex;const code=root.querySelector('[data-text-hex]');if(code)code.textContent=hex;}
    for(const b of swatches?.querySelectorAll('button')||[])b.classList.toggle('active',level===100&&b.dataset.hex===rgbToHex(rgb));
    if(save){try{localStorage.setItem(STORAGE_KEY,JSON.stringify({rgb,level}));}catch(_){}}
  }

  if(swatches){
    swatches.innerHTML=TEXT_COLOR_PRESETS.map(p=>`<button type="button" class="text-swatch" data-hex="${p.hex}" title="${p.name}" aria-label="${p.name}" style="--swatch:${p.hex}"></button>`).join('');
    swatches.addEventListener('click',e=>{
      const b=e.target instanceof Element?e.target.closest('button[data-hex]'):null;if(!b)return;
      rgb=hexToRgb(b.dataset.hex);level=100;apply();
    });
  }
  ['r','g','b'].forEach((k,i)=>sliders[k]?.addEventListener('input',()=>{rgb[i]=clamp(sliders[k].value,0,255);apply();}));
  bright?.addEventListener('input',()=>{level=clamp(bright.value,20,100);apply();});
  root.querySelector('[data-text-reset]')?.addEventListener('click',()=>{rgb=[...DEFAULT_RGB];level=100;apply();});

  apply(false);
}
