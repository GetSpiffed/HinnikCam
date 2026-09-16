#pragma once
#include <Arduino.h>
const char WEB_PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="nl">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#101b17">
<title>SpecialCam</title>
<style>
:root{color-scheme:dark;--bg:#101b17;--panel:#192720;--line:#304239;--text:#eff4ec;--muted:#a5b6aa;--green:#c3e697;--danger:#f1afa2}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--text);font:15px/1.5 system-ui,-apple-system,sans-serif}
button,a{-webkit-tap-highlight-color:transparent}button{font:inherit;cursor:pointer}a{color:var(--green);text-underline-offset:4px}
button:focus-visible,a:focus-visible,summary:focus-visible{outline:3px solid var(--green);outline-offset:4px}
button:disabled{opacity:.45;cursor:default}svg{width:20px;height:20px;flex-shrink:0;fill:none;stroke:currentColor;stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round}
.shell{width:min(100%,864px);margin:auto;padding:24px 24px 28px}
header{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:26px}
.brand{display:flex;align-items:center;gap:11px}.mark{width:42px;height:42px;border:1px solid #53704d;border-radius:14px;display:grid;place-items:center;color:var(--green);background:#233225}.mark svg{width:25px;height:25px}
h1{font-size:21px;letter-spacing:-.7px;margin:0;line-height:1.2}.tagline{font-size:12px;color:var(--muted);margin:3px 0 0}
.local{font-size:11px;letter-spacing:.08em;text-transform:uppercase;color:var(--muted);white-space:nowrap}
.view-heading{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:12px}
h2{font-size:15px;font-weight:600;margin:0}.badge{display:inline-flex;gap:7px;align-items:center;background:#24352a;color:var(--green);padding:5px 10px;border-radius:30px;font-size:11px;font-weight:650;letter-spacing:.02em}
.badge:before{content:"";width:6px;height:6px;border-radius:50%;background:currentColor}
.badge[data-state=offline]{background:#3c2826;color:var(--danger)}.badge[data-state=waiting]{background:#2b3027;color:#d3ccac}
.viewer{position:relative;overflow:hidden;border:1px solid var(--line);border-radius:18px;background:#080e0b}
#camera{display:block;width:100%;aspect-ratio:4/3;object-fit:contain;font-size:14px;color:var(--muted)}
.viewer-label{position:absolute;bottom:13px;left:14px;border:1px solid #ffffff24;border-radius:6px;padding:4px 8px;background:#0a130dcc;color:#dfebdf;font-size:10px;letter-spacing:.08em;pointer-events:none}
.status-line{font-size:12px;color:var(--muted);margin:10px 2px 16px;min-height:18px}
.actions{display:flex;gap:10px}.actions button{min-height:49px;border-radius:12px;display:flex;align-items:center;justify-content:center;gap:9px;padding:12px 18px;font-weight:650}
.primary{flex:1;background:var(--green);color:#162210;border:1px solid var(--green)}.secondary{background:var(--panel);border:1px solid var(--line);color:var(--text)}
.action-note{font-size:12px;color:var(--muted);margin:10px 2px 0;min-height:20px}.action-note a{margin-left:8px}
.metrics{display:grid;grid-template-columns:1fr 1fr;gap:12px;margin:22px 0 18px}
.metric{background:var(--panel);border:1px solid var(--line);border-radius:14px;padding:16px 18px;min-width:0}
.metric-label{display:flex;align-items:center;gap:7px;color:var(--muted);font-size:11px;letter-spacing:.06em;text-transform:uppercase;margin-bottom:7px}.metric-label svg{width:15px;height:15px}
.metric strong{display:block;font-size:18px;font-weight:550;letter-spacing:-.3px}.metric small{display:block;color:var(--muted);font-size:12px;margin-top:3px}
details{border-top:1px solid var(--line);border-bottom:1px solid var(--line);font-size:13px}
summary{cursor:pointer;padding:14px 2px;color:var(--muted)}.details-body{padding:0 2px 16px;color:var(--muted)}.details-body p{margin:0 0 8px}.details-body a{display:inline-block;padding:5px 0}
footer{display:flex;align-items:center;justify-content:space-between;gap:16px;margin-top:20px}
footer p{font-size:11px;color:var(--muted);margin:0}.power-button{display:flex;align-items:center;gap:7px;background:none;border:1px solid #65483f;border-radius:10px;min-height:44px;padding:9px 13px;color:var(--danger);font-size:12px;white-space:nowrap}
.power-button svg{width:16px;height:16px}
@media(min-width:700px){.shell{padding-top:32px}.actions .primary{flex:0 1 240px}header{margin-bottom:32px}.metrics{margin-top:24px}}
@media(max-width:420px){.shell{padding:18px 16px 24px}header{margin-bottom:23px}.local{font-size:9px}.actions button{padding:12px;font-size:13px}.metric{padding:14px 12px}.metric strong{font-size:16px}.metric small{font-size:11px}footer{gap:10px}footer p{max-width:145px}}
@media(prefers-reduced-motion:no-preference){button{transition:background .15s,opacity .15s}.primary:hover:not(:disabled){background:#d3f1b0}}
[hidden]{display:none!important}
</style>
</head>
<body>
<main class="shell">
<header>
 <div class="brand"><span class="mark" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M5 19V9l4-5 3 2 4-2 2 7-5 3v5M9 4V2m7 2 2-2M5 10l8 4m-3 5h6"/><path d="M13 8h.01"/></svg></span><div><h1>SpecialCam</h1><p class="tagline">Zicht op je paard.</p></div></div>
 <span class="local">Direct via wifi</span>
</header>
<section aria-label="Live camera">
 <div class="view-heading"><h2>In de trailer</h2><span id="badge" class="badge" data-state="waiting">Verbinden</span></div>
 <div class="viewer"><img id="camera" alt="Camerabeeld uit de trailer"><span class="viewer-label">TRAILERCAMERA · 640 × 480</span></div>
 <p id="status" class="status-line" role="status">Verbinding met de camera maken…</p>
 <div class="actions">
  <button id="capture" class="primary" disabled><svg aria-hidden="true" viewBox="0 0 24 24"><path d="M4 6h4l2-3h4l2 3h4v14H4z"/><circle cx="12" cy="12.5" r="3.5"/></svg><span id="capture-label">Foto opslaan</span></button>
  <button id="restart" class="secondary"><svg aria-hidden="true" viewBox="0 0 24 24"><path d="M20 10a8 8 0 1 0-2 8M20 4v6h-6"/></svg>Opnieuw verbinden</button>
 </div>
 <p class="action-note" aria-live="polite"><span id="photo-status">Bewaar een camerabeeld op je telefoon.</span><a id="photo-open" hidden target="_blank" rel="noopener">Open foto</a></p>
</section>
<section class="metrics" aria-label="Apparaatstatus">
 <div class="metric"><div class="metric-label"><svg aria-hidden="true" viewBox="0 0 24 24"><path d="M2 8a16 16 0 0 1 20 0M5 12a11 11 0 0 1 14 0M8 16a6 6 0 0 1 8 0M12 20h.01"/></svg>Verbinding</div><strong id="connection">SpecialCam</strong><small id="clients">Status ophalen…</small></div>
 <div class="metric"><div class="metric-label"><svg aria-hidden="true" viewBox="0 0 24 24"><rect x="2" y="6" width="17" height="12" rx="2"/><path d="M22 10v4M6 10v4m4-4v4"/></svg>Voeding</div><strong id="power">Status ophalen…</strong><small id="battery">Accustatus ophalen…</small></div>
</section>
<details><summary>Verbindingsgegevens</summary><div class="details-body"><p id="info">IP-adres wordt opgehaald.</p><p>Geen internet nodig. Stilstaand beeld? Kies opnieuw verbinden.</p><a href="/stream" target="_blank" rel="noopener">Directe camerastream openen ↗</a></div></details>
<footer><p>Lokaal verbonden.<br>Geen cloud. Geen opname.</p><button id="shutdown" class="power-button"><svg aria-hidden="true" viewBox="0 0 24 24"><path d="M12 2v10M6 5a9 9 0 1 0 12 0"/></svg>Uitschakelen</button></footer>
</main>
<script>
const el=id=>document.getElementById(id);
const camera=el('camera'), status=el('status');
let shuttingDown=false, cameraReady=false, capturing=false, photoUrl=null;
function badge(text,state){el('badge').textContent=text;el('badge').dataset.state=state;}
function syncCapture(){el('capture').disabled=shuttingDown || capturing || !cameraReady;}
function start(){
  if(shuttingDown)return;
  badge('Verbinden','waiting');
  status.textContent='Verbinding met de camera maken…';
  camera.src='/stream?t='+Date.now();
}
camera.onerror=()=>{
  if(shuttingDown)return;
  badge('Onderbroken','offline');
  status.textContent='Beeld onderbroken. Kies opnieuw verbinden.';
};
el('restart').onclick=start;
el('capture').onclick=async()=>{
  if(shuttingDown || capturing || !cameraReady)return;
  capturing=true;syncCapture();el('capture-label').textContent='Foto maken…';
  el('photo-open').hidden=true;
  el('photo-status').textContent='Een camerabeeld ophalen…';
  const controller=new AbortController(), timer=setTimeout(()=>controller.abort(),10000);
  try{
    const response=await fetch('/capture',{cache:'no-store',signal:controller.signal});
    if(!response.ok)throw Error();
    const blob=await response.blob();
    if(!blob.size || !blob.type.startsWith('image/jpeg'))throw Error();
    if(shuttingDown)return;
    if(photoUrl)URL.revokeObjectURL(photoUrl);
    photoUrl=URL.createObjectURL(blob);
    const download=document.createElement('a');
    download.href=photoUrl;
    download.download='SpecialCam-'+new Date().toISOString().replace(/[:.]/g,'-')+'.jpg';
    document.body.appendChild(download);download.click();download.remove();
    el('photo-open').href=photoUrl;el('photo-open').hidden=false;
    el('photo-status').textContent='Download gestart. Lukt opslaan niet?';
  }catch{
    if(!shuttingDown)el('photo-status').textContent='Foto niet opgehaald. Probeer opnieuw.';
  }finally{
    clearTimeout(timer);capturing=false;syncCapture();el('capture-label').textContent='Foto opslaan';
  }
};
function showShutdown(){
  shuttingDown=true;syncCapture();
  camera.removeAttribute('src');
  el('shutdown').disabled=true;el('restart').disabled=true;
  badge('Uitschakelen','waiting');
  status.textContent='SpecialCam schakelt uit. Aanzetten met PWRKEY op het board.';
}
el('shutdown').onclick=async()=>{
  if(!confirm('SpecialCam uitschakelen? Wifi en camera stoppen. Weer aanzetten kan alleen bij het board.'))return;
  el('shutdown').disabled=true;
  const controller=new AbortController(), timer=setTimeout(()=>controller.abort(),5000);
  try{
    const response=await fetch('/shutdown',{method:'POST',headers:{'X-SpecialCam-Confirm':'yes'},signal:controller.signal});
    if(!response.ok)throw Error();
    showShutdown();
  }catch{
    if(!shuttingDown){status.textContent='Uitschakelen niet bevestigd. Controleer het board of probeer opnieuw.';el('shutdown').disabled=false;}
  }finally{clearTimeout(timer);}
};
async function poll(){
  if(shuttingDown)return;
  const controller=new AbortController(), timer=setTimeout(()=>controller.abort(),2500);
  try{
    const r=await fetch('/status',{cache:'no-store',signal:controller.signal});
    if(!r.ok)throw Error();
    const s=await r.json();
    if(shuttingDown || s.shutting_down){showShutdown();return;}
    cameraReady=s.camera_ready;syncCapture();
    el('power').textContent=!s.power_ready?'Onbekend':s.usb?'USB-voeding':'Accuvoeding';
    el('battery').textContent=!s.power_ready?'Geen voedingsgegevens':
      s.battery && s.battery_mv>0?'Accu '+(s.battery_mv/1000).toFixed(2)+' V'+(s.charging?' · Laden':''):'Geen accumeting';
    el('connection').textContent='SpecialCam';
    el('clients').textContent=s.clients+' '+(s.clients===1?'apparaat verbonden':'apparaten verbonden');
    el('info').textContent='Adres: http://'+(s.hostname || s.ip)+' · IP: '+s.ip+' · VGA 640 × 480 · JPEG';
    if(!s.camera_ready){badge('Camerafout','offline');status.textContent='Camera niet beschikbaar. Controleer het board en herstart.';}
    else if(s.streaming && s.frame_age_ms<3000){badge('Live','live');status.textContent='Verbonden · De camera verstuurt beelden.';}
    else{badge('Wachten op beeld','waiting');status.textContent='Verbonden · Nog geen recente beelden. Verbind zo nodig opnieuw.';}
  }catch{
    if(!shuttingDown){cameraReady=false;syncCapture();badge('Offline','offline');status.textContent='Geen verbinding. Verbind je telefoon met SpecialCam-wifi.';el('connection').textContent='Niet bereikbaar';el('clients').textContent='Controleer de wifi-verbinding';}
  }finally{clearTimeout(timer);if(!shuttingDown)setTimeout(poll,2000);}
}
start();poll();
</script>
</body></html>)HTML";
