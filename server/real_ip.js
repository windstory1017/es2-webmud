// 把玩家的真實 IP 告訴遊戲。
//
// 玩家經 Caddy（https 轉送）→ 橋接程式 → 遊戲，遊戲看到的位址都是本機，分不出誰是誰。
// 橋接程式在每條連線一開始先送一行「@@REALIP <密碼> <真實IP> <同一戶代表>」，
// 遊戲（/adm/daemons/realipd.c）核對密碼後記下來，供同 IP 在線人數限制、封鎖 IP 等功能使用。
// 密碼存在 mudlib/adm/etc/bridge_secret（不放進 git），第一次執行時自動產生。

import fs from 'node:fs';
import net from 'node:net';
import path from 'node:path';
import crypto from 'node:crypto';

const LOOPBACK = new Set(['127.0.0.1', '::1']);

function normalize(ip) {
  ip = String(ip || '').trim();
  if (ip.startsWith('[') && ip.includes(']')) ip = ip.slice(1, ip.indexOf(']'));
  if (/^::ffff:\d+\.\d+\.\d+\.\d+$/i.test(ip)) ip = ip.slice(7);
  const zone = ip.indexOf('%');
  if (zone >= 0) ip = ip.slice(0, zone);
  return net.isIP(ip) ? ip.toLowerCase() : '';
}

// 連線來自本機的轉送程式（Caddy）時，才採用 X-Forwarded-For 的最後一個位址（轉送程式自己加上的）；
// 外部直接連進來的連線，標頭可能是玩家偽造的，一律用連線本身的位址。
export function realIpOf(req) {
  const remote = normalize(req.socket?.remoteAddress);
  if (LOOPBACK.has(remote)) {
    const xff = String(req.headers?.['x-forwarded-for'] || '').split(',').map(normalize).filter(Boolean);
    if (xff.length) return xff[xff.length - 1];
  }
  return remote;
}

function expandIpv6(ip) {
  let [head, tail] = ip.includes('::') ? ip.split('::') : [ip, null];
  const h = head ? head.split(':') : [];
  const t = tail ? tail.split(':') : [];
  // 結尾是 IPv4 寫法（例如 ::1.2.3.4）時換成兩組十六進位
  const last = (t.length ? t : h);
  if (last.length && last[last.length - 1].includes('.')) {
    const v4 = last.pop().split('.').map(Number);
    last.push(((v4[0] << 8) | v4[1]).toString(16), ((v4[2] << 8) | v4[3]).toString(16));
  }
  const fill = tail === null ? [] : Array(8 - h.length - t.length).fill('0');
  return [...h, ...fill, ...t].map(x => parseInt(x || '0', 16).toString(16));
}

// 同一戶的代表：IPv4 就是位址本身；IPv6 同一戶的裝置常拿到不同位址，以前 64 位元合併。
export function ipGroup(ip) {
  if (net.isIPv4(ip)) return ip;
  if (net.isIPv6(ip)) return expandIpv6(ip).slice(0, 4).join(':') + '::/64';
  return '';
}

export function loadBridgeSecret(mudlibDir) {
  const file = path.join(mudlibDir, 'adm', 'etc', 'bridge_secret');
  try {
    const s = fs.readFileSync(file, 'utf8').trim();
    if (/^[0-9a-f]{32,}$/.test(s)) return s;
  } catch {}
  const s = crypto.randomBytes(24).toString('hex');
  try {
    fs.mkdirSync(path.dirname(file), {recursive: true});
    fs.writeFileSync(file, s + '\n', {mode: 0o600});
    return s;
  } catch {
    return '';
  }
}

// 本機直接連線（本機測試、單機版）不送：遊戲照舊把大家當成本機，不受人數限制。
export function realIpLine(req, secret) {
  if (!secret) return '';
  const ip = realIpOf(req);
  if (!ip || LOOPBACK.has(ip)) return '';
  return `@@REALIP ${secret} ${ip} ${ipGroup(ip)}\r\n`;
}

// 玩家自己輸入的內容不能冒充暗號。
export function scrubRealIp(buf) {
  const s = buf.toString('latin1');
  return s.includes('@@REALIP') ? Buffer.from(s.replaceAll('@@REALIP', '@@realip'), 'latin1') : buf;
}
