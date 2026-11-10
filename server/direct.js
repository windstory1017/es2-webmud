// zMUD／Telnet 直連入口。設計文件：docs/ZMUD_TELNET_PLAN.md
//
//   zMUD 4.62    ──telnet:4000（Big5）──▶ 本程式（轉碼）──▶ 遊戲伺服器 Neolith（4001，只給本機）
//   UTF-8 用戶端 ──telnet:4003（UTF-8）─▶ 本程式（原樣轉接）─▶ 同上
//
// 只做連線轉接與編碼轉換，不含任何遊戲規則。出問題時把 ZMUD_PORT、TELNET_PORT 設為 0 就關閉。

import net from 'node:net';
import {StringDecoder} from 'node:string_decoder';
import iconv from 'iconv-lite';
import {realIpLine, scrubRealIp} from './real_ip.js';

const IAC = 255, SB = 250, SE = 240;
const WILL = 251, WONT = 252, DO = 253, DONT = 254;

// 把位元組流切成「文字」與「telnet 控制訊號」兩種片段。控制訊號（0xFF 開頭）不可轉碼。
// 狀態會跨封包保留，所以控制訊號被切成兩段送達也不會壞。
export class TelnetSplitter {
  constructor(onText, onRaw) {
    this.onText = onText;
    this.onRaw = onRaw;
    this.state = 'data';
  }
  feed(buf) {
    let text = [], raw = [];
    const flushText = () => { if (text.length) { this.onText(Buffer.from(text)); text = []; } };
    const flushRaw = () => { if (raw.length) { this.onRaw(Buffer.from(raw)); raw = []; } };
    for (const b of buf) {
      switch (this.state) {
        case 'data':
          if (b === IAC) { flushText(); raw.push(b); this.state = 'iac'; }
          else text.push(b);
          break;
        case 'iac':
          raw.push(b);
          if (b === SB) this.state = 'sb';
          else if (b === WILL || b === WONT || b === DO || b === DONT) this.state = 'opt';
          else { this.state = 'data'; flushRaw(); }   // IAC IAC（資料中的 0xFF）與單一位元組指令
          break;
        case 'opt':
          raw.push(b); this.state = 'data'; flushRaw();
          break;
        case 'sb':
          raw.push(b);
          if (b === IAC) this.state = 'sbiac';
          break;
        case 'sbiac':
          raw.push(b);
          if (b === SE) { this.state = 'data'; flushRaw(); }
          else this.state = 'sb';   // IAC IAC 仍在子協商內
          break;
      }
    }
    flushText(); flushRaw();
  }
}

// 一條直連的雙向管線。big5=true 時轉碼（遊戲 UTF-8 ⇄ 玩家 Big5/CP950），否則原樣轉接。
// 串流式解碼：一個中文字被切成兩段送達時先暫存，湊齊再轉；Big5 沒有的字變成「?」。
export function makePipes(big5) {
  if (!big5) return {down: b => b, up: b => b};
  const gameText = new StringDecoder('utf8');
  const clientEnc = iconv.getEncoder('cp950');
  const clientDec = iconv.getDecoder('cp950');
  const gameEnc = iconv.getEncoder('utf8');
  const mk = (decode, encode) => {
    let out = [];
    const sp = new TelnetSplitter(
      t => { const s = decode(t); if (s) out.push(encode(s)); },
      r => out.push(r),
    );
    return b => { out = []; sp.feed(b); return Buffer.concat(out); };
  };
  return {
    down: mk(t => gameText.write(t), s => clientEnc.write(s)),   // 遊戲 → 玩家
    up: mk(t => clientDec.write(t), s => gameEnc.write(s)),      // 玩家 → 遊戲
  };
}

export function startDirectListener({name, port, host, big5, mudHost, mudPort, maxSessions, connectTimeoutMs, bridgeSecret, trace}) {
  if (!port) return null;
  const live = new Set();
  const say = (sock, msg) => {
    if (sock.destroyed) return;
    try { sock.write(big5 ? iconv.encode(msg, 'cp950') : Buffer.from(msg, 'utf8')); } catch {}
  };
  const server = net.createServer(client => {
    const remote = client.remoteAddress || '';
    if (live.size >= maxSessions) {
      say(client, '\r\n[伺服器人數已滿，請稍後再試]\r\n');
      client.end();
      return;
    }
    client.setNoDelay(true);
    client.setKeepAlive(true, 60000);
    const pipes = makePipes(big5);
    const tcp = net.createConnection({host: mudHost, port: mudPort});
    tcp.setNoDelay(true);
    live.add(client);
    let connected = false;
    // 第一行先告訴遊戲玩家的真實 IP（直連的來源位址就是玩家本人，沒有轉送標頭可偽造）。
    const ipLine = realIpLine({socket: {remoteAddress: remote}, headers: {}}, bridgeSecret);
    if (ipLine) tcp.write(ipLine);
    const timer = setTimeout(() => { if (!connected) tcp.destroy(new Error('connect timeout')); }, connectTimeoutMs);
    const done = () => { clearTimeout(timer); live.delete(client); if (!client.destroyed) client.end(); if (!tcp.destroyed) tcp.destroy(); };

    tcp.on('connect', () => { connected = true; clearTimeout(timer); trace(`${name} ${remote} connected`); });
    tcp.on('data', d => {
      if (client.destroyed) return;
      if (client.writableLength > 1048576) { client.destroy(); tcp.destroy(); return; }   // 玩家端太慢
      const out = pipes.down(d);
      if (out.length) client.write(out);
    });
    tcp.on('error', e => {
      trace(`${name} ${remote} mud error: ${e.code || e.message}`);
      if (!connected) say(client, '\r\n[遊戲伺服器目前無法連線，可能正在重開，請稍後再試]\r\n');
    });
    tcp.on('close', done);
    client.on('data', d => {
      if (tcp.destroyed) return;
      const out = scrubRealIp(pipes.up(d));
      if (out.length) tcp.write(out);
    });
    client.on('error', () => {});
    client.on('close', done);
  });
  server.on('error', e => trace(`${name} 無法在 ${host}:${port} 監聽：${e.code || e.message}（此入口不啟用，其他功能不受影響）`));
  server.listen(port, host, () => trace(`${name} listening ${host}:${port} -> ${mudHost}:${mudPort}`));
  server.sessions = live;
  return server;
}
