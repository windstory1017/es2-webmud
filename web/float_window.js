// 浮動視窗（地圖、角色狀態、介面設定）：左側選單按鈕切換開關，拖曳標題列移動位置；
// 地圖可拉四個角落調整大小，角色狀態大小固定。開關狀態、位置與大小記在瀏覽器。

let topZ=40;
// 按到哪個視窗，那個視窗就疊到最上面（聊天視窗也一起排）。
export function bringToFront(el){if(el)el.style.zIndex=String(++topZ);}

export function setupFloatWindow({root,openButton,storageKey,resizable=false,defaultGeometry,defaultOpen=false,rememberOpen=true,onShow}){
  if(!root)return {open(){},close(){},isOpen:()=>false};
  const head=root.querySelector('[data-float-drag]');
  let geometry=null,isOpen=defaultOpen;

  try{
    const saved=JSON.parse(localStorage.getItem(storageKey)||'null');
    if(saved&&typeof saved==='object'){
      if(rememberOpen&&typeof saved.open==='boolean')isOpen=saved.open;
      if(saved.geometry&&typeof saved.geometry==='object')geometry=saved.geometry;
    }
  }catch(_){}
  function persist(){
    try{localStorage.setItem(storageKey,JSON.stringify({open:!root.hidden,geometry}));}catch(_){}
  }

  function clampIntoView(){
    const r=root.getBoundingClientRect();
    const maxLeft=Math.max(0,window.innerWidth-Math.min(r.width,window.innerWidth));
    const maxTop=Math.max(0,window.innerHeight-48);
    root.style.left=Math.min(Math.max(0,r.left),maxLeft)+'px';
    root.style.top=Math.min(Math.max(0,r.top),maxTop)+'px';
  }
  function applyGeometry(){
    const g=geometry||defaultGeometry?.()||{left:16,top:16};
    root.style.left=g.left+'px';root.style.top=g.top+'px';
    if(resizable){
      if(g.width)root.style.width=g.width+'px';
      if(g.height)root.style.height=g.height+'px';
    }
    clampIntoView();
  }
  function saveGeometry(){
    const r=root.getBoundingClientRect();
    geometry={left:Math.round(r.left),top:Math.round(r.top)};
    if(resizable){geometry.width=Math.round(r.width);geometry.height=Math.round(r.height);}
    persist();
  }

  let drag=null;
  root.addEventListener('pointerdown',()=>bringToFront(root));
  head?.addEventListener('pointerdown',e=>{
    if(e.button!==0||(e.target instanceof Element&&e.target.closest('button')))return;
    const r=root.getBoundingClientRect();
    drag={dx:e.clientX-r.left,dy:e.clientY-r.top,id:e.pointerId};
    head.setPointerCapture(e.pointerId);
    root.classList.add('is-dragging');
    e.preventDefault();
  });
  head?.addEventListener('pointermove',e=>{
    if(!drag||e.pointerId!==drag.id)return;
    root.style.left=(e.clientX-drag.dx)+'px';
    root.style.top=(e.clientY-drag.dy)+'px';
    clampIntoView();
  });
  const endDrag=e=>{
    if(!drag||e.pointerId!==drag.id)return;
    drag=null;root.classList.remove('is-dragging');
    saveGeometry();
  };
  head?.addEventListener('pointerup',endDrag);
  head?.addEventListener('pointercancel',endDrag);

  // 拖拉四個角落調整大小（對角固定不動），放開後記住。最小尺寸照 CSS 的 min-width／min-height。
  if(resizable){
    for(const corner of ['nw','ne','sw','se']){
      const grip=document.createElement('div');
      grip.className=`float-resize float-resize-${corner}`;grip.setAttribute('aria-hidden','true');
      root.appendChild(grip);
      let rs=null;
      grip.addEventListener('pointerdown',e=>{
        if(e.button!==0)return;
        const r=root.getBoundingClientRect(),cs=getComputedStyle(root);
        rs={id:e.pointerId,x:e.clientX,y:e.clientY,r,minW:parseFloat(cs.minWidth)||200,minH:parseFloat(cs.minHeight)||150};
        grip.setPointerCapture(e.pointerId);
        root.classList.add('is-resizing');
        e.preventDefault();e.stopPropagation();
      });
      grip.addEventListener('pointermove',e=>{
        if(!rs||e.pointerId!==rs.id)return;
        const dx=e.clientX-rs.x,dy=e.clientY-rs.y,{r}=rs;
        let {left,top,width,height}=r;
        if(corner.includes('e'))width=r.width+dx;
        if(corner.includes('w'))width=r.width-dx;
        if(corner.includes('s'))height=r.height+dy;
        if(corner.includes('n'))height=r.height-dy;
        width=Math.min(Math.max(rs.minW,width),window.innerWidth);
        height=Math.min(Math.max(rs.minH,height),window.innerHeight);
        if(corner.includes('w'))left=r.right-width;
        if(corner.includes('n'))top=r.bottom-height;
        root.style.left=Math.max(0,left)+'px';root.style.top=Math.max(0,top)+'px';
        root.style.width=width+'px';root.style.height=height+'px';
      });
      const endResize=e=>{
        if(!rs||e.pointerId!==rs.id)return;
        rs=null;root.classList.remove('is-resizing');
        saveGeometry();
      };
      grip.addEventListener('pointerup',endResize);
      grip.addEventListener('pointercancel',endResize);
    }
  }
  window.addEventListener('resize',()=>{if(!root.hidden)clampIntoView();});

  function setButton(on){
    openButton?.classList.toggle('active',on);openButton?.setAttribute('aria-pressed',on?'true':'false');
  }
  function open(){
    root.hidden=false;
    applyGeometry();bringToFront(root);setButton(true);persist();
    onShow?.();
  }
  function close(){
    root.hidden=true;setButton(false);persist();
  }
  root.querySelector('[data-float-close]')?.addEventListener('click',close);
  openButton?.addEventListener('click',()=>{if(root.hidden)open();else close();});

  if(isOpen)open();else setButton(false);
  return {open,close,isOpen:()=>!root.hidden};
}

// 訊息欄與最右側空白欄之間的分隔線：左右拖曳分配寬度，按兩下恢復預設。
export function setupSideSplitter({shell,splitter,storageKey,defaultWidth=420,minCenter=()=>360}){
  if(!shell||!splitter)return {refresh(){}};
  // desired 是玩家拉出來的寬度；視窗變窄時暫時縮小，變回寬時恢復。
  let desired=defaultWidth;
  try{const v=localStorage.getItem(storageKey),w=Number(v);if(v!==null&&Number.isFinite(w)&&w>=0)desired=w;}catch(_){}
  function maxWidth(){
    const nav=shell.querySelector('.game-nav')?.getBoundingClientRect().width||0;
    return Math.max(0,shell.clientWidth-nav-splitter.offsetWidth-minCenter());
  }
  function apply(w){
    const width=Math.round(Math.max(0,Math.min(maxWidth(),w)));
    shell.style.setProperty('--side-width',width+'px');
    return width;
  }
  function save(){try{localStorage.setItem(storageKey,String(desired));}catch(_){}}
  let drag=null;
  splitter.addEventListener('pointerdown',e=>{
    if(e.button!==0)return;
    drag={id:e.pointerId};
    splitter.setPointerCapture(e.pointerId);
    document.body.classList.add('is-resizing-side');
    e.preventDefault();
  });
  splitter.addEventListener('pointermove',e=>{
    if(!drag||e.pointerId!==drag.id)return;
    desired=apply(shell.getBoundingClientRect().right-e.clientX-splitter.offsetWidth/2);
  });
  const end=e=>{
    if(!drag||e.pointerId!==drag.id)return;
    drag=null;document.body.classList.remove('is-resizing-side');save();
  };
  splitter.addEventListener('pointerup',end);
  splitter.addEventListener('pointercancel',end);
  splitter.addEventListener('dblclick',()=>{desired=defaultWidth;apply(desired);save();});
  window.addEventListener('resize',()=>apply(desired));
  apply(desired);
  return {refresh:()=>apply(desired)};
}
