// 「回報」視窗：讀寫遊戲裡的「問題回報與建議」留言板（設計文件：docs/WEB_BOARD.md）。
// 請求跟著背景狀態查詢送出（webhud board …），回覆是 @@WEBHUD|B… 資料行，由 app.js 交給 onHud()。
// 只有登入角色後才能使用；發文者就是登入中的角色。

const TITLE_MAX=20,BODY_MAX=1000,CHUNK_BYTES=1200,REQUEST_TIMEOUT_MS=8000,COUNT_EVERY_MS=60000;
const CATS={bug:'回報',idea:'建議'};
const esc=s=>String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const unescapeText=s=>String(s||'').replace(/\\(.)/g,(m,c)=>c==='n'?'\n':c==='p'?'|':c);
// 送往遊戲的文字：反斜線、換行、空白都跳脫，整段才會是一行、前後空白也不會被吃掉。
function escapeTokens(text){
  const out=[];
  for(const ch of String(text).replace(/\r/g,'')){
    out.push(ch==='\\'?'\\\\':ch==='\n'?'\\n':ch===' '?'\\s':ch);
  }
  return out;
}
const enc=new TextEncoder();
function chunkTokens(tokens){
  const chunks=[];let cur='',bytes=0;
  for(const t of tokens){
    const b=enc.encode(t).length;
    if(bytes+b>CHUNK_BYTES&&cur){chunks.push(cur);cur='';bytes=0;}
    cur+=t;bytes+=b;
  }
  if(cur||!chunks.length)chunks.push(cur);
  return chunks;
}
function splitTitle(title){
  const m=String(title).match(/^(Re﹕)?【(回報|建議)】(.*)$/);
  if(!m)return {cat:'',text:String(title)};
  return {cat:m[2]==='建議'?'idea':'bug',text:(m[1]||'')+m[3]};
}
function shortDate(t){const d=new Date(t*1000);return `${d.getMonth()+1}/${d.getDate()}`;}
function fullDate(t){const d=new Date(t*1000),p=n=>String(n).padStart(2,'0');return `${d.getFullYear()}/${p(d.getMonth()+1)}/${p(d.getDate())} ${p(d.getHours())}:${p(d.getMinutes())}`;}

export function setupBoard({root,openButton,badge,sendRequest,isReady}){
  if(!root)return {onHud(){return false;}};
  const body=root.querySelector('[data-board-body]');
  const titleEl=root.querySelector('[data-board-title]');
  let seq=0;
  const pending=new Map();     // 請求代號 → {resolve,reject,timer,notes,read}
  let notes=[],stats={total:0,unread:0,admin:false},filter='all',view='list',current=null,draft=null,busy=false;

  // ---------- 與遊戲溝通 ----------
  function request(action,args=''){
    return new Promise((resolve,reject)=>{
      if(!isReady()){reject(new Error('登入角色後才能使用回報板。'));return;}
      const req='b'+(++seq);
      const timer=setTimeout(()=>{pending.delete(req);reject(new Error('連線忙碌，請稍後再試一次。'));},REQUEST_TIMEOUT_MS);
      pending.set(req,{resolve,reject,timer,notes:[],read:null});
      sendRequest(`board ${action} ${req}${args?' '+args:''}`);
    });
  }
  // app.js 每收到一段狀態回覆就會從頭再解析一次，所以這裡要能承受同一行重複出現。
  function onHud(a){
    const kind=a[1];
    if(kind!=='BOARDBEGIN'&&kind!=='BNOTE'&&kind!=='BREAD'&&kind!=='BOARDEND')return false;
    if(kind==='BOARDBEGIN'){const p=pending.get(a[2]);if(p){p.notes=[];p.read=null;p.active=true;}lastReq=a[2];return true;}
    const p=pending.get(lastReq);
    if(kind==='BNOTE'&&p&&a.length>=8){
      p.notes.push({num:+a[2],time:+a[3],read:a[4]==='1',mine:a[5]==='1',author:a[6],title:a.slice(7).join('|')});
      return true;
    }
    if(kind==='BREAD'&&p&&a.length>=8){
      p.read={num:+a[2],time:+a[3],mine:a[4]==='1',author:a[5],title:a[6],msg:unescapeText(a.slice(7).join('|'))};
      return true;
    }
    if(kind==='BOARDEND'){
      const q=pending.get(a[2]);if(!q)return true;
      pending.delete(a[2]);clearTimeout(q.timer);
      if(a[3]!=='ok'){q.reject(new Error(a.slice(4).join('|')||'發生錯誤，請再試一次。'));return true;}
      stats={total:+a[4]||0,unread:+a[5]||0,admin:a[6]==='1',capacity:+a[7]||0};
      updateBadge();
      q.resolve({notes:q.notes,read:q.read});
      return true;
    }
    return true;
  }
  let lastReq='';

  function updateBadge(){
    if(badge){badge.textContent=stats.unread>99?'99+':String(stats.unread);badge.hidden=!stats.unread;}
    if(titleEl)titleEl.textContent=stats.total?`${stats.total} 篇／${stats.unread} 篇未讀`:'';
  }

  // ---------- 畫面 ----------
  function setBusy(on){busy=on;root.classList.toggle('is-busy',on);}
  function showError(msg){
    const box=body.querySelector('[data-board-error]');
    if(box){box.textContent=msg;box.hidden=!msg;}
  }
  function tag(cat){return cat?`<span class="board-tag board-tag-${cat}">${CATS[cat]}</span>`:'<span class="board-tag"></span>';}

  function renderList(){
    view='list';
    const shown=notes.slice().reverse().filter(n=>{
      if(filter==='unread')return !n.read;
      if(filter==='bug'||filter==='idea')return splitTitle(n.title).cat===filter;
      return true;
    });
    const btn=(id,label)=>`<button type="button" data-board-filter="${id}" class="${filter===id?'active':''}">${label}</button>`;
    body.innerHTML=`<div class="board-bar">${btn('all','全部')}${btn('bug','回報')}${btn('idea','建議')}${btn('unread','只看未讀')}<span class="board-gap"></span><button type="button" class="primary" data-board-compose>＋ 發文</button></div>
      <div class="board-error" data-board-error hidden></div>
      <div class="board-list">${shown.length?shown.map(n=>{const t=splitTitle(n.title);return `<button type="button" class="board-row${n.read?'':' unread'}" data-board-open="${n.num}" data-time="${n.time}"><span class="board-num">${n.num}</span>${tag(t.cat)}<span class="board-subject">${esc(t.text)}</span><span class="board-author">${esc(n.author)}</span><span class="board-date">${shortDate(n.time)}</span></button>`;}).join(''):`<div class="board-empty">${notes.length?'沒有符合的文章。':'目前還沒有任何文章。'}</div>`}</div>
      <div class="board-foot">點標題閱讀；未讀的標題是黃色，最新的在最上面。板上最多保留 ${stats.capacity||100} 篇，超過時最舊的會被清掉。</div>`;
  }
  function renderRead(){
    view='read';
    const n=current,t=splitTitle(n.title);
    const order=notes.slice().reverse(),i=order.findIndex(x=>x.time===n.time);
    const lines=n.msg.replace(/\n+$/,'').split('\n').map(l=>/^>\s?/.test(l)?`<span class="board-quote">${esc(l)}</span>`:esc(l)).join('\n');
    body.innerHTML=`<div class="board-bar"><button type="button" data-board-back>‹ 返回列表</button></div>
      <div class="board-error" data-board-error hidden></div>
      <article class="board-article"><h3>${tag(t.cat)} ${esc(t.text)}</h3><div class="board-meta">[${n.num}]　${esc(n.author)}　${fullDate(n.time)}</div><div class="board-text">${lines}</div></article>
      <div class="board-bar"><button type="button" data-board-nav="${i-1}" ${i>0?'':'disabled'}>‹ 較新</button><button type="button" data-board-nav="${i+1}" ${i>=0&&i<order.length-1?'':'disabled'}>較舊 ›</button><span class="board-gap"></span><button type="button" data-board-reply>回覆</button>${n.mine||stats.admin?'<button type="button" data-board-delete>刪除</button>':''}</div>`;
  }
  function renderCompose(){
    view='compose';
    const d=draft;
    const cat=(id)=>`<button type="button" data-board-cat="${id}" class="${d.cat===id?'active':''}">${CATS[id]}</button>`;
    body.innerHTML=`<div class="board-bar"><button type="button" data-board-back>‹ 返回列表</button><strong class="board-compose-title">${d.reply?'回覆文章':'發表新文章'}</strong></div>
      <div class="board-error" data-board-error hidden></div>
      <div class="board-form">
        <div class="board-field"><span>類別</span><div class="board-cats">${cat('bug')}${cat('idea')}</div></div>
        <label class="board-field"><span>標題（${d.reply?'回覆可略長':TITLE_MAX+' 字以內'}）</span><input data-board-input-title maxlength="${d.reply?TITLE_MAX+4:TITLE_MAX}" value="${esc(d.title)}" spellcheck="false"></label>
        <label class="board-field board-field-grow"><span>內容 <small data-board-counter></small></span><textarea data-board-input-body maxlength="${BODY_MAX}" spellcheck="false">${esc(d.body)}</textarea></label>
        <div class="board-bar"><span class="board-note">以目前登入的角色發文</span><span class="board-gap"></span><button type="button" data-board-back>取消</button><button type="button" class="primary" data-board-send>送出</button></div>
      </div>`;
    updateCounter();
    body.querySelector(d.reply?'[data-board-input-body]':'[data-board-input-title]')?.focus();
  }
  function updateCounter(){
    const ta=body.querySelector('[data-board-input-body]'),c=body.querySelector('[data-board-counter]');
    if(ta&&c)c.textContent=`${[...ta.value].length}／${BODY_MAX}`;
  }

  // ---------- 動作 ----------
  async function loadList(){
    setBusy(true);
    try{const r=await request('list');notes=r.notes;if(view==='list')renderList();}
    catch(e){if(view==='list'){renderList();showError(e.message);}}
    finally{setBusy(false);}
  }
  async function openNote(num,time){
    setBusy(true);
    try{
      const r=await request('read',`${num} ${time}`);
      if(!r.read)throw new Error('這篇留言已經不在板上了。');
      current=r.read;
      const n=notes.find(x=>x.time===current.time);if(n)n.read=true;
      renderRead();
    }catch(e){showError(e.message);}
    finally{setBusy(false);}
  }
  function startCompose(reply){
    if(reply){
      const t=splitTitle(current.title);
      // 引用原文開頭幾行就好（最多 6 行、約 200 字），不然長文一引用就超過字數上限。
      let quote=current.msg.split('\n').filter(l=>l&&!/^>\s?/.test(l));
      const sig=quote.findIndex(l=>l.startsWith('--'));if(sig>=0)quote.length=sig;
      let room=200;const cut=[];
      for(const l of quote.slice(0,6)){if(room<=0)break;const c=[...l];cut.push(c.length>room?c.slice(0,room).join('')+'…':l);room-=c.length;}
      if(cut.length<quote.length||room<0)cut.push('…');
      quote=cut;
      draft={reply:true,cat:t.cat||'bug',title:(/^Re﹕/.test(t.text)?t.text:'Re﹕'+t.text).slice(0,TITLE_MAX+4),
        body:`> ${current.author} 在 (${fullDate(current.time).slice(0,10)}) 留下這篇留言﹕\n${quote.map(l=>'> '+l).join('\n')}\n\n`};
    }else draft={reply:false,cat:'bug',title:'',body:''};
    renderCompose();
  }
  async function send(){
    const title=body.querySelector('[data-board-input-title]').value.trim();
    const text=body.querySelector('[data-board-input-body]').value.replace(/\s+$/,'');
    draft.title=title;draft.body=text;
    if(!title){showError('請填寫標題。');return;}
    if(!text.trim()){showError('請填寫內容。');return;}
    if([...text].length>BODY_MAX){showError(`內容太長了，請在 ${BODY_MAX} 字以內。`);return;}
    setBusy(true);
    try{
      const chunks=chunkTokens(escapeTokens(text));
      for(let i=0;i<chunks.length;i++)await request('chunk',`${i} ${chunks[i]}`);
      await request('post',`${draft.cat} ${escapeTokens(title).join('')}`);
      draft=null;filter='all';view='list';
      await loadList();
    }catch(e){showError(e.message);}
    finally{setBusy(false);}
  }
  async function remove(){
    if(!confirm('確定要刪除這篇文章嗎？刪除後無法復原。'))return;
    setBusy(true);
    try{await request('del',`${current.num} ${current.time}`);view='list';await loadList();}
    catch(e){showError(e.message);}
    finally{setBusy(false);}
  }

  body.addEventListener('click',e=>{
    const b=e.target instanceof Element?e.target.closest('button'):null;
    if(!b||busy||b.disabled)return;
    if(b.dataset.boardFilter){filter=b.dataset.boardFilter;renderList();}
    else if(b.hasAttribute('data-board-compose'))startCompose(false);
    else if(b.dataset.boardOpen)openNote(b.dataset.boardOpen,b.dataset.time);
    else if(b.hasAttribute('data-board-back')){draft=null;renderList();loadList();}
    else if(b.dataset.boardNav!==undefined){const n=notes.slice().reverse()[+b.dataset.boardNav];if(n)openNote(n.num,n.time);}
    else if(b.hasAttribute('data-board-reply'))startCompose(true);
    else if(b.hasAttribute('data-board-delete'))remove();
    else if(b.dataset.boardCat){draft.cat=b.dataset.boardCat;for(const x of body.querySelectorAll('[data-board-cat]'))x.classList.toggle('active',x===b);}
    else if(b.hasAttribute('data-board-send'))send();
  });
  body.addEventListener('input',e=>{if(e.target.matches?.('[data-board-input-body]'))updateCounter();});

  // 打開視窗時重新抓列表；平常每分鐘查一次未讀數，有新文章時左側按鈕亮紅色數字。
  function onOpen(){if(view==='list'||!view)renderList();loadList();}
  setInterval(()=>{if(isReady()&&root.hidden)request('count').catch(()=>{});},COUNT_EVERY_MS);
  let firstCount=setInterval(()=>{if(isReady()){clearInterval(firstCount);request('count').catch(()=>{});}},3000);
  renderList();
  return {onHud,onOpen};
}
