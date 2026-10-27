#!/usr/bin/env node
import net from 'node:net';
import {spawn} from 'node:child_process';
import WebSocket from '../server/node_modules/ws/index.js';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const here=path.dirname(fileURLToPath(import.meta.url));
const root=path.resolve(here,'..');
const mudPort=14500+Math.floor(Math.random()*250);
const webPort=18500+Math.floor(Math.random()*250);
const wait=ms=>new Promise(r=>setTimeout(r,ms));
let tcpSeq=0;
const mud=net.createServer(sock=>{
  const id=++tcpSeq;
  sock.setNoDelay(true);
  sock.write(`WELCOME:${id}\r\n`);
  sock.on('data',d=>sock.write(`ECHO:${id}:${String(d)}`));
});
await new Promise((resolve,reject)=>{mud.once('error',reject);mud.listen(mudPort,'127.0.0.1',resolve)});
const child=spawn(process.execPath,['server/index.js'],{
  cwd:root,
  env:{...process.env,HOST:'127.0.0.1',PORT:String(webPort),MUD_HOST:'127.0.0.1',MUD_PORT:String(mudPort),MAX_SESSIONS:'64',HEARTBEAT_MS:'5000'},
  stdio:['ignore','pipe','pipe']
});
let log='';child.stdout.on('data',d=>log+=d);child.stderr.on('data',d=>log+=d);
for(let i=0;i<100&&!log.includes('ES2 WebMUD');i++)await wait(50);
if(!log.includes('ES2 WebMUD'))throw new Error('bridge did not start: '+log);

async function client(label){
  const payload=`<${label}>`;
  return new Promise((resolve,reject)=>{
    const ws=new WebSocket(`ws://127.0.0.1:${webPort}/mud`);let text='';let tcp=null;
    const timer=setTimeout(()=>{try{ws.close()}catch{};reject(new Error(`timeout ${label}: ${text}`))},6000);
    ws.on('message',d=>{
      text+=String(d);
      const m=text.match(/WELCOME:(\d+)/);if(m)tcp=Number(m[1]);
      if(tcp&&text.includes(`ECHO:${tcp}:${payload}`)){clearTimeout(timer);ws.close();resolve({label,tcp,text})}
    });
    ws.on('open',()=>ws.send(payload+'\r\n'));
    ws.on('error',e=>{clearTimeout(timer);reject(e)});
  });
}

const N=50;
const first=await Promise.all(Array.from({length:N},(_,i)=>client(`burst-${i+1}`)));
const ids=first.map(x=>x.tcp);
const unique=new Set(ids).size===N;
let crossTalk=false;
for(const r of first){
  for(let j=1;j<=N;j++)if(r.label!==`burst-${j}`&&r.text.includes(`<burst-${j}>`)){crossTalk=true;break}
}

// The bridge frees a session when the websocket close handshake finishes, so on a
// slow machine the previous round can still hold slots; wait (bounded) for it to drain.
const health=()=>fetch(`http://127.0.0.1:${webPort}/api/multiplayer-health`).then(r=>r.json());
async function waitIdle(){for(let i=0;i<100;i++){if((await health()).sessions.active===0)return;await wait(50)}}

// Rapid connect/disconnect churn checks cleanup and re-use pressure.
let churnOk=true;
for(let round=1;round<=4;round++){
  await waitIdle();
  const rows=await Promise.all(Array.from({length:20},(_,i)=>client(`churn-${round}-${i+1}`)));
  if(new Set(rows.map(x=>x.tcp)).size!==20)churnOk=false;
  await wait(100);
}
await waitIdle();
const finalHealth=await health();
const cleaned=finalHealth.sessions.active===0 && finalHealth.sessions.connected===0;
const result={ok:unique&&!crossTalk&&churnOk&&cleaned,burstClients:N,uniqueTcpSessions:new Set(ids).size,crossTalk,churnRounds:4,churnClientsPerRound:20,cleanupAfterChurn:cleaned,health:finalHealth};
console.log(JSON.stringify(result,null,2));
child.kill('SIGTERM');await wait(100);await new Promise(r=>mud.close(r));
process.exit(result.ok?0:2);
