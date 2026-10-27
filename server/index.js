import http from 'node:http';
import net from 'node:net';
import fs from 'node:fs/promises';
import fsSync from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {WebSocketServer} from 'ws';
import {TelnetCodec} from './telnet.js';
import {spawn} from 'node:child_process';
import crypto from 'node:crypto';
import {AdminJobState} from './admin_jobs.js';
import {loadBridgeSecret,realIpLine,scrubRealIp} from './real_ip.js';

const __dirname=path.dirname(fileURLToPath(import.meta.url));
const ROOT=path.resolve(__dirname,'..');
const WEB=path.join(ROOT,'web');
const UPSTREAM=path.resolve(process.env.ES2_REPO || path.join(ROOT,'source','upstream'));
const MUDLIB=path.join(UPSTREAM,'mudlib');
const BUILD_VERSION=(()=>{try{return fsSync.readFileSync(path.join(ROOT,'VERSION'),'utf8').trim()}catch{return 'unknown'}})();
const CATALOG=path.join(ROOT,'catalog','generated');
const HOST=process.env.HOST||'127.0.0.1';
const PORT=Number(process.env.PORT||8080);
const MUD_HOST=process.env.MUD_HOST||'127.0.0.1';
const MUD_PORT=Number(process.env.MUD_PORT||4000);
const MAX_SESSIONS=Math.max(1,Number(process.env.MAX_SESSIONS||200));
const TCP_CONNECT_TIMEOUT_MS=Math.max(1000,Number(process.env.TCP_CONNECT_TIMEOUT_MS||10000));
const WS_MAX_PAYLOAD_BYTES=Math.max(1024,Number(process.env.WS_MAX_PAYLOAD_BYTES||65536));
const WS_MAX_BUFFERED_BYTES=Math.max(65536,Number(process.env.WS_MAX_BUFFERED_BYTES||1048576));
const HEARTBEAT_MS=Math.max(5000,Number(process.env.HEARTBEAT_MS||30000));
const bridgeStats={startedAt:Date.now(),accepted:0,rejectedCapacity:0,tcpErrors:0,wsErrors:0,peakSessions:0};
const adminJobs=new AdminJobState();
const mime={'.html':'text/html; charset=utf-8','.js':'text/javascript; charset=utf-8','.css':'text/css; charset=utf-8','.json':'application/json; charset=utf-8','.c':'text/plain; charset=utf-8','.h':'text/plain; charset=utf-8','.woff2':'font/woff2','.txt':'text/plain; charset=utf-8','.md':'text/plain; charset=utf-8'};


// 經過反向代理或 Tunnel（Caddy、cloudflared）轉送的請求，來源看起來也是本機，
// 但會帶轉送標頭；這種請求一律視為外部連線，管理功能不開放。
const PROXY_HEADERS=['x-forwarded-for','forwarded','x-real-ip','cf-connecting-ip','cf-ray'];
function isLocal(req){
  if(PROXY_HEADERS.some(h=>req.headers[h]!==undefined))return false;
  return ['127.0.0.1','::1','::ffff:127.0.0.1'].includes(req.socket.remoteAddress||'');
}
function runProcess(cmd,args,{cwd=ROOT,timeout=120000}={}){
  return new Promise((resolve)=>{
    const p=spawn(cmd,args,{cwd,stdio:['ignore','pipe','pipe']});
    let stdout='',stderr='',killed=false;
    const cap=s=>s.length>200000?s.slice(-200000):s;
    p.stdout.on('data',d=>stdout=cap(stdout+d));
    p.stderr.on('data',d=>stderr=cap(stderr+d));
    const timer=setTimeout(()=>{killed=true;p.kill('SIGKILL')},timeout);
    p.on('close',code=>{clearTimeout(timer);resolve({ok:code===0&&!killed,code,stdout,stderr,killed})});
  });
}

function send(res,code,body,type='text/plain; charset=utf-8'){res.writeHead(code,{'content-type':type,'cache-control':'no-store'});res.end(body)}
function safeWithin(base,rel){const out=path.resolve(base,(rel||'').replace(/^\/+/,''));if(out!==base&&!out.startsWith(base+path.sep))throw new Error('path rejected');return out}
async function bodyText(req){let s='';for await(const c of req){s+=c;if(s.length>2000000)throw new Error('body too large')}return s}

const server=http.createServer(async(req,res)=>{
  try{
    const u=new URL(req.url,'http://localhost');


    if(u.pathname==='/api/version'){
      return send(res,200,JSON.stringify({version:BUILD_VERSION,projectRoot:ROOT,mudHost:MUD_HOST,mudPort:MUD_PORT},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/health'){
      return send(res,200,JSON.stringify({ok:true,bridge:{host:HOST,port:PORT},mud:{host:MUD_HOST,port:MUD_PORT},sessions:sessions?.size??0,connected:[...(sessions?.values?.()||[])].filter(x=>x.tcpConnected).length,maxSessions:MAX_SESSIONS,peakSessions:bridgeStats.peakSessions},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/runtime-probe'&&req.method==='POST'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','runtime_probe.py'),'--host',MUD_HOST,'--port',String(MUD_PORT)],{timeout:15000});
      return send(res,x.ok?200:503,x.stdout||x.stderr,'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/runtime-readiness'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','runtime_readiness.py'),UPSTREAM,'--host',MUD_HOST,'--port',String(MUD_PORT)],{timeout:15000});
      return send(res,x.ok?200:500,x.stdout||x.stderr,'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/admin/verify-source'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','verify_source.py'),UPSTREAM],{timeout:30000});
      return send(res,x.ok?200:409,JSON.stringify(x,null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/admin/jobs'){
      if(!isLocal(req))return send(res,403,'localhost only');
      return send(res,200,JSON.stringify(adminJobs.snapshot(),null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/admin/reindex'&&req.method==='POST'){
      if(!isLocal(req))return send(res,403,'localhost only');
      let job;
      try{job=adminJobs.start('reindex',{requestedBy:req.socket.remoteAddress||''})}
      catch(e){
        if(e.code==='JOB_BUSY')return send(res,409,JSON.stringify({ok:false,error:'reindex already running',job:e.job},null,2),'application/json; charset=utf-8');
        throw e;
      }
      try{
        adminJobs.stage(job,'verify_source');
        const verify=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','verify_source.py'),UPSTREAM],{timeout:30000});
        if(!verify.ok){
          adminJobs.finish(job,false,{stage:'verify_source',verify});
          return send(res,409,JSON.stringify({ok:false,job,verify},null,2),'application/json; charset=utf-8');
        }
        adminJobs.stage(job,'build_catalog');
        const build=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','build_catalog.py'),UPSTREAM,'-o',CATALOG],{timeout:120000});
        adminJobs.finish(job,build.ok,{stage:'build_catalog',verify,build});
        return send(res,build.ok?200:500,JSON.stringify({ok:build.ok,job,verify,build},null,2),'application/json; charset=utf-8');
      }catch(e){
        adminJobs.finish(job,false,{stage:'exception',error:e.message});
        throw e;
      }
    }
    if(u.pathname==='/api/admin/backups'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','backup_ops.py'),ROOT,'list'],{timeout:15000});
      return send(res,x.ok?200:500,x.stdout||x.stderr,'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/admin/backup-diff'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const backup=u.searchParams.get('backup')||'',current=u.searchParams.get('current')||'';
      if(!backup||!current)return send(res,400,'backup and current required');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','backup_ops.py'),ROOT,'diff',backup,current],{timeout:15000});
      return send(res,x.ok?200:400,x.stdout||x.stderr,'text/plain; charset=utf-8');
    }
    if(u.pathname==='/api/admin/restore'&&req.method==='POST'){
      if(!isLocal(req))return send(res,403,'localhost only');
      const data=JSON.parse(await bodyText(req)||'{}');
      if(!data.backup||!data.current)return send(res,400,'backup and current required');
      const verify=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','verify_source.py'),UPSTREAM],{timeout:30000});
      if(!verify.ok)return send(res,409,JSON.stringify({ok:false,stage:'verify_source',verify},null,2),'application/json; charset=utf-8');
      const x=await runProcess(process.env.PYTHON||'python',[path.join(ROOT,'tools','backup_ops.py'),ROOT,'restore',data.backup,data.current],{timeout:15000});
      return send(res,x.ok?200:400,x.stdout||x.stderr,'application/json; charset=utf-8');
    }

    if(u.pathname==='/api/status'){
      const sourceOK=fsSync.existsSync(path.join(MUDLIB,'std','char.c'))&&fsSync.existsSync(path.join(MUDLIB,'feature','char','combat.c'));
      let catalog=null;try{catalog=JSON.parse(await fs.readFile(path.join(CATALOG,'manifest.json'),'utf8'))}catch{}
      return send(res,200,JSON.stringify({sourceOK,upstream:UPSTREAM,mudHost:MUD_HOST,mudPort:MUD_PORT,catalog},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/online-count'){
      const connected=[...sessions.values()].filter(s=>s.tcpConnected).length;
      return send(res,200,JSON.stringify({count:connected,connecting:sessions.size-connected,capacity:MAX_SESSIONS},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/multiplayer-health'){
      const connected=[...sessions.values()].filter(s=>s.tcpConnected).length;
      return send(res,200,JSON.stringify({
        ok:true,
        uptimeSeconds:Math.floor((Date.now()-bridgeStats.startedAt)/1000),
        sessions:{active:sessions.size,connected,connecting:sessions.size-connected,capacity:MAX_SESSIONS,peak:bridgeStats.peakSessions},
        totals:{accepted:bridgeStats.accepted,rejectedCapacity:bridgeStats.rejectedCapacity,tcpErrors:bridgeStats.tcpErrors,wsErrors:bridgeStats.wsErrors},
        limits:{websocketPayloadBytes:WS_MAX_PAYLOAD_BYTES,slowClientBufferedBytes:WS_MAX_BUFFERED_BYTES,heartbeatMs:HEARTBEAT_MS}
      },null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/sessions'){
      if(!isLocal(req))return send(res,403,'localhost only');
      return send(res,200,JSON.stringify({count:sessions.size,capacity:MAX_SESSIONS,sessions:[...sessions.values()].map(s=>({...s,ageSeconds:Math.floor((Date.now()-s.started)/1000)}))},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/catalog'){
      const f=safeWithin(CATALOG,u.searchParams.get('file')||'manifest.json');
      return send(res,200,await fs.readFile(f),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/search'){
      const q=(u.searchParams.get('q')||'').trim().toLowerCase();
      const type=(u.searchParams.get('type')||'').trim();
      if(!q && !type)return send(res,400,'q or type required');
      const rows=JSON.parse(await fs.readFile(path.join(CATALOG,'all.json'),'utf8'));
      const hit=rows.filter(r=>{
        if(type&&r.type!==type)return false;
        if(!q)return true;
        const hay=[r.path,r.file,r.short,r.name,r.command,...(r.ids||[])].filter(Boolean).join(' ').toLowerCase();
        return hay.includes(q);
      }).slice(0,500);
      return send(res,200,JSON.stringify({count:hit.length,results:hit},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/world-audit'){return send(res,200,await fs.readFile(path.join(CATALOG,'world_audit.json')),'application/json; charset=utf-8');}
    if(u.pathname==='/api/triage'){return send(res,200,await fs.readFile(path.join(CATALOG,'triage.json')),'application/json; charset=utf-8');}
    if(u.pathname==='/api/source-status'){return send(res,200,await fs.readFile(path.join(CATALOG,'source_status.json')),'application/json; charset=utf-8');}
    if(u.pathname==='/api/dependency'){
      const graph=JSON.parse(await fs.readFile(path.join(CATALOG,'dependency_graph.json'),'utf8'));
      const pth=u.searchParams.get('path');
      return send(res,200,JSON.stringify(pth?(graph.entities[pth]||{incoming:[],outgoing:[]}):graph,null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/relations'){
      const pth=u.searchParams.get('path')||'';
      if(!pth)return send(res,400,'path required');
      const graph=JSON.parse(await fs.readFile(path.join(CATALOG,'reference_graph.json'),'utf8'));
      return send(res,200,JSON.stringify(graph.entities[pth]||{incoming:[],outgoing:[]},null,2),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/index'){
      const kind=(u.searchParams.get('kind')||'').trim();
      const allowed=new Set(['npc','item','skill','vendor','command','equipment','combat_npc','combat_core']);
      if(!allowed.has(kind))return send(res,400,'unsupported kind');
      const file=kind==='command'?'command_index.json':kind+'_index.json';
      return send(res,200,await fs.readFile(path.join(CATALOG,file)),'application/json; charset=utf-8');
    }
    if(u.pathname==='/api/entity'){
      const pth=u.searchParams.get('path')||'';
      if(!pth)return send(res,400,'path required');
      const rows=JSON.parse(await fs.readFile(path.join(CATALOG,'all.json'),'utf8'));
      const hit=rows.find(r=>r.path===pth);
      return hit?send(res,200,JSON.stringify(hit,null,2),'application/json; charset=utf-8'):send(res,404,'not found');
    }
    if(u.pathname==='/api/source'&&req.method==='GET'){
      // 原始碼與 mudlib/data（玩家存檔，含密碼雜湊和 email）只給主機本機的管理頁讀取。
      if(!isLocal(req))return send(res,403,'Source viewing is localhost-only');
      const rel=u.searchParams.get('path')||'';
      if(!rel.startsWith('mudlib/'))return send(res,400,'Only mudlib/* is editable');
      return send(res,200,await fs.readFile(safeWithin(UPSTREAM,rel),'utf8'),'text/plain; charset=utf-8');
    }
    if(u.pathname==='/api/source'&&req.method==='PUT'){
      if(!isLocal(req))return send(res,403,'Source editing is localhost-only');
      const rel=u.searchParams.get('path')||'';
      if(!rel.startsWith('mudlib/'))return send(res,400,'Only mudlib/* is editable');
      const f=safeWithin(UPSTREAM,rel),text=await bodyText(req);
      const before=await fs.readFile(f).catch(()=>Buffer.alloc(0));
      const beforeSha=before.length?crypto.createHash('sha256').update(before).digest('hex'):null;
      const afterBuf=Buffer.from(text,'utf8');
      const afterSha=crypto.createHash('sha256').update(afterBuf).digest('hex');
      const stamp=new Date().toISOString().replace(/[:.]/g,'-');
      const backup=path.join(ROOT,'patches',stamp+'__'+rel.replace(/[\\/]/g,'__')+'.bak');
      await fs.mkdir(path.dirname(backup),{recursive:true});
      if(before.length)await fs.writeFile(backup,before);
      await fs.writeFile(f,afterBuf);
      return send(res,200,JSON.stringify({saved:true,backup:before.length?path.relative(ROOT,backup):null,beforeSha256:beforeSha,afterSha256:afterSha},null,2),'application/json; charset=utf-8');
    }
    const rel=u.pathname==='/'?'index.html':u.pathname.replace(/^\/+/,''), file=safeWithin(WEB,rel);
    return send(res,200,await fs.readFile(file),mime[path.extname(file)]||'application/octet-stream');
  }catch(e){return send(res,e.code==='ENOENT'?404:500,e.message)}
});

const sessions=new Map();
let sessionSeq=0;
const bridgeLog=path.join(ROOT,'reports','bridge_runtime.log');
// 每個指令封包都記一行只在除錯時需要（BRIDGE_TRACE_PACKETS=1）：平常 30 人在線每秒要寫幾十行，
// 紀錄檔會長到數百 MB，而且同步寫檔會拖慢轉送。連線建立、關閉、錯誤照常記錄。
const TRACE_PACKETS=process.env.BRIDGE_TRACE_PACKETS==='1';
const BRIDGE_SECRET=loadBridgeSecret(MUDLIB);
function bridgeTrace(line){
  const text=`${new Date().toISOString()} ${line}\n`;
  try{fsSync.mkdirSync(path.dirname(bridgeLog),{recursive:true});fsSync.appendFileSync(bridgeLog,text)}catch{}
  console.log('[bridge]',line);
}
const wss=new WebSocketServer({server,path:'/mud',maxPayload:WS_MAX_PAYLOAD_BYTES});
wss.on('connection',(ws,req)=>{
  if(sessions.size>=MAX_SESSIONS){
    bridgeStats.rejectedCapacity++;
    try{ws.send('\r\n[bridge] server at capacity, please retry later\r\n')}catch{}
    ws.close(1013,'server at capacity');return;
  }
  const id=++sessionSeq, started=Date.now();
  bridgeStats.accepted++;
  ws.isAlive=true;
  const tcp=net.createConnection({host:MUD_HOST,port:MUD_PORT});
  tcp.setNoDelay(true);
  // 第一行先告訴遊戲玩家的真實 IP（排在玩家任何輸入之前）。
  const realIp=realIpLine(req,BRIDGE_SECRET);
  if(realIp)tcp.write(realIp);
  const codec=new TelnetCodec(b=>{if(!tcp.destroyed)tcp.write(b)});
  const row={id,started,remote:req.socket.remoteAddress||'',bytesIn:0,bytesOut:0,lastActivity:started,tcpConnected:false,closing:false};
  sessions.set(id,row);
  bridgeStats.peakSessions=Math.max(bridgeStats.peakSessions,sessions.size);
  const cleanup=()=>{if(row.closing)return;row.closing=true;sessions.delete(id);};
  const connectTimer=setTimeout(()=>{if(!row.tcpConnected&&!tcp.destroyed)tcp.destroy(new Error('MUD connect timeout'))},TCP_CONNECT_TIMEOUT_MS);
  tcp.on('connect',()=>{row.tcpConnected=true;row.lastActivity=Date.now();clearTimeout(connectTimer);bridgeTrace(`session ${id} TCP connected to ${MUD_HOST}:${MUD_PORT}`);if(ws.readyState===1)ws.send('\r\n[ES2 connected]\r\n')});
  tcp.on('data',d=>{
    row.bytesOut+=d.length;row.lastActivity=Date.now();
    const clean=codec.feed(d);
    if(!clean.length||ws.readyState!==1)return;
    if(ws.bufferedAmount>WS_MAX_BUFFERED_BYTES){
      try{ws.close(1013,'client too slow')}catch{}
      if(!tcp.destroyed)tcp.destroy();
      return;
    }
    ws.send(clean);
  });
  tcp.on('error',e=>{bridgeStats.tcpErrors++;clearTimeout(connectTimer);bridgeTrace(`session ${id} TCP error: ${e.message}`);if(ws.readyState===1)ws.send(`\r\n[bridge] ${e.message}\r\n`)});
  tcp.on('close',hadError=>{clearTimeout(connectTimer);bridgeTrace(`session ${id} TCP closed hadError=${hadError} bytesOut=${row.bytesOut} bytesIn=${row.bytesIn}`);cleanup();if(ws.readyState===1)ws.close(1011,'MUD TCP closed')});
  ws.on('pong',()=>{ws.isAlive=true});
  ws.on('message',m=>{
    const b=scrubRealIp(Buffer.from(m));row.bytesIn+=b.length;row.lastActivity=Date.now();
    // Length/timing only: useful for diagnosing pre-login automation without
    // ever recording usernames, passwords or command contents.
    if(TRACE_PACKETS)bridgeTrace(`session ${id} WS->MUD packet bytes=${b.length} total=${row.bytesIn}`);
    if(!tcp.destroyed)tcp.write(b);
  });
  ws.on('error',()=>{bridgeStats.wsErrors++});
  ws.on('close',()=>{clearTimeout(connectTimer);cleanup();if(!tcp.destroyed)tcp.destroy()});
});
const ping=setInterval(()=>{
  for(const ws of wss.clients){
    if(ws.isAlive===false){ws.terminate();continue;}
    ws.isAlive=false;if(ws.readyState===1)ws.ping();
  }
},HEARTBEAT_MS);
server.on('close',()=>clearInterval(ping));
const shutdown=()=>{
  clearInterval(ping);
  for(const ws of wss.clients){try{ws.close(1001,'server shutdown')}catch{}}
  server.close(()=>process.exit(0));
  setTimeout(()=>process.exit(0),3000).unref();
};
process.once('SIGINT',shutdown);
process.once('SIGTERM',shutdown);
server.listen(PORT,HOST,()=>console.log(`ES2 WebMUD http://${HOST}:${PORT} -> ${MUD_HOST}:${MUD_PORT}`));
