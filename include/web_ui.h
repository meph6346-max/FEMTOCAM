// FEMTOCAM — web_ui.h
// Copyright (c) 2025 Gorogepapa (meph6346@gmail.com) — MIT License
#pragma once
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<title>FEMTOCAM</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg:#1e1e2e;--sf:#282838;--s2:#313145;--bd:#3b3b50;--tx:#cdd6f4;--t2:#8888a8;
--ac:#e84427;--gn:#4caf50;--bl:#42a5f5;--yl:#ffc107;--rd:#ef5350;--r:4px}
body{background:var(--bg);color:var(--tx);font:13px/1.4 -apple-system,'Segoe UI',Roboto,sans-serif;max-width:520px;margin:0 auto}
.hd{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;background:var(--sf);border-bottom:1px solid var(--bd)}
.lo{font-size:15px;font-weight:600}.lo b{color:var(--ac)}
.mb{padding:2px 8px;border-radius:2px;font-weight:600;font-size:10px;margin-left:10px}
.mb-a{background:var(--ac);color:#fff}.mb-s{background:var(--gn);color:#fff}
.tb{display:flex;background:var(--sf);border-bottom:1px solid var(--bd)}
.tab{flex:1;text-align:center;padding:8px;font-size:11px;color:var(--t2);cursor:pointer;border-bottom:2px solid transparent}
.tab.on{color:var(--ac);border-color:var(--ac)}
.pg{display:none}.pg.on{display:block}
.cam{width:100%;aspect-ratio:4/3;background:#111;position:relative;overflow:hidden}
.cam img{width:100%;height:100%;object-fit:contain}
.cam-msg{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;color:var(--t2);font-size:12px}
.badge{position:absolute;top:8px;left:8px;background:rgba(232,68,39,.85);color:#fff;font:600 10px/1 sans-serif;padding:3px 8px;border-radius:2px;display:none}
.badge.on{display:block;animation:rp 1.5s infinite}@keyframes rp{50%{opacity:.4}}
.fbtn{position:absolute;top:8px;right:8px;width:30px;height:30px;border-radius:50%;background:rgba(0,0,0,.5);border:1.5px solid rgba(255,255,255,.2);color:var(--t2);display:flex;align-items:center;justify-content:center;cursor:pointer}
.fbtn.on{background:rgba(255,193,7,.25);border-color:var(--yl);color:var(--yl)}
.restag{position:absolute;bottom:8px;right:8px;background:rgba(0,0,0,.6);color:var(--t2);font-size:10px;padding:2px 6px;border-radius:2px}
.pn{padding:12px 14px}
.row3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:6px;margin-bottom:8px}
.stat{background:var(--sf);border-radius:var(--r);padding:10px 8px;text-align:center}
.stat b{font-size:16px;display:block}.stat small{font-size:10px;color:var(--t2)}
.card{background:var(--sf);border-radius:var(--r);padding:14px 14px;margin-bottom:10px}
.ctt{font-size:11px;color:var(--t2);text-transform:uppercase;letter-spacing:.8px;margin-bottom:10px}
.rw{display:flex;align-items:center;gap:10px;min-height:34px}.rw+.rw{margin-top:10px}
.lb{font-size:12px;color:var(--t2);white-space:nowrap;flex:1}.vl{font-size:12px;font-weight:500;text-align:right}
.rc{display:flex;align-items:center;gap:5px;justify-content:flex-end;min-width:160px}
.tip{font-size:9px;color:var(--t2);margin-top:2px;margin-bottom:6px;line-height:1.4;padding-left:0}
select,input{background:var(--s2);color:var(--tx);border:1px solid var(--bd);border-radius:var(--r);font:13px/1.4 inherit;outline:0}
select{padding:5px 8px;min-width:75px}input{padding:8px 10px;width:100%}
input[type=range]{padding:0;width:90px}input[type=number]{width:56px;text-align:center;padding:6px}
.unit{font-size:11px;color:var(--t2);white-space:nowrap}
select:focus,input:focus{border-color:var(--ac)}
.btns{display:flex;gap:6px}
.btn{flex:1;padding:8px;border:none;border-radius:var(--r);font:600 12px/1 inherit;cursor:pointer;text-align:center}
.btn:active{opacity:.7}.btn:disabled{opacity:.4;cursor:default}
.btn-p{background:var(--ac);color:#fff}
.btn-g{background:var(--s2);color:var(--tx);border:1px solid var(--bd)}
.flist{background:var(--sf);border-radius:var(--r)}
.fitem{display:flex;align-items:center;padding:10px 14px;border-bottom:1px solid var(--bd)}
.fitem:last-child{border-bottom:0}
.fchk{width:16px;height:16px;accent-color:var(--ac);cursor:pointer;margin-right:8px;flex-shrink:0}
.finfo{flex:1;min-width:0}.fname{font-size:12px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.fsize{font-size:11px;color:var(--t2)}
.frec{font-size:9px;background:var(--ac);color:#fff;padding:1px 5px;border-radius:2px;margin-left:6px}
.fbtns{display:flex;gap:2px;flex-shrink:0}
.fbtns button{background:0;border:0;color:var(--t2);cursor:pointer;font-size:11px;padding:2px 6px}
.fbtns button:hover{color:var(--ac)}.fbtns .del{color:var(--rd)}.fbtns .ren{color:var(--bl)}
.frename{background:var(--s2);border:1px solid var(--ac);color:var(--tx);border-radius:var(--r);padding:2px 6px;font:12px inherit;width:100%}
.fd{margin-bottom:10px}.fl{font-size:11px;color:var(--t2);margin-bottom:4px}
.wlist{background:var(--s2);border:1px solid var(--bd);border-radius:var(--r);max-height:150px;overflow-y:auto;margin-bottom:10px}
.witem{display:flex;align-items:center;justify-content:space-between;padding:10px 12px;cursor:pointer;border-bottom:1px solid var(--bd)}
.witem:last-child{border-bottom:0}.witem:hover{background:rgba(232,68,39,.08)}.witem.sel{background:rgba(232,68,39,.15)}
.wrssi{font-size:10px;color:var(--t2)}
.iptog{display:flex;border:1px solid var(--bd);border-radius:var(--r);overflow:hidden;margin-bottom:10px}
.ipo{flex:1;text-align:center;padding:6px;font-size:12px;cursor:pointer;background:var(--s2);color:var(--t2);border:0}
.ipo.on{background:var(--ac);color:#fff}
.sip{display:none}.sip.sh{display:block}
.sr{display:flex;gap:8px}.sr>div{flex:1}
.scb{background:0;border:1px solid var(--bd);color:var(--t2);border-radius:var(--r);padding:4px 10px;font-size:10px;cursor:pointer}
.scb:hover{border-color:var(--ac);color:var(--tx)}
.ft{padding:8px 14px;border-top:1px solid var(--bd);display:flex;justify-content:space-between;font-size:10px;color:var(--t2)}
.empty{padding:16px;text-align:center;color:var(--t2);font-size:12px}
.warn{background:rgba(239,83,80,.1);border:1px solid rgba(239,83,80,.3);border-radius:var(--r);padding:8px 12px;margin-bottom:8px;font-size:11px;color:var(--rd)}
.hint{font-size:10px;margin-top:2px}
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:var(--sf);border:1px solid var(--bd);color:var(--tx);padding:8px 16px;border-radius:var(--r);font-size:12px;opacity:0;transition:opacity .3s;pointer-events:none;z-index:99}
.toast.err{border-color:var(--rd);color:var(--rd)}.toast.show{opacity:1}
.langbtn{background:0;border:1px solid var(--bd);color:var(--t2);border-radius:var(--r);padding:2px 6px;font-size:10px;cursor:pointer;margin-left:6px}
.langbtn:hover{color:var(--tx);border-color:var(--ac)}
.logbox{background:#0a0a12;border:1px solid var(--bd);border-radius:var(--r);padding:6px;font:10px/1.4 'Consolas','Courier New',monospace;color:var(--gn);max-height:200px;overflow-y:auto;white-space:pre-wrap;word-break:break-all}
</style></head><body>

<div class="hd">
 <div style="display:flex;align-items:center"><div class="lo" id="logoName">Femto<b>cam</b></div><span class="mb mb-a" id="M">AP</span></div>
 <div style="display:flex;align-items:center"><span style="font-size:11px;color:var(--t2)" id="MI"></span><button class="langbtn" onclick="toggleLang()" id="langBtn">KO</button></div>
</div>

<div class="tb">
 <div class="tab on" onclick="T('cam')" data-i="tab_cam">Camera</div>
 <div class="tab" onclick="T('files')" data-i="tab_files">Files</div>
 <div class="tab" onclick="T('cfg')" data-i="tab_cfg">Settings</div>
</div>

<!-- ======== Camera Tab ======== -->
<div class="pg on" id="p-cam">
 <div class="cam"><img id="S" style="display:none"><div class="cam-msg" id="SE" data-i="connecting">Connecting...</div>
  <div class="badge" id="RB">REC</div>
  <div class="fbtn" id="FB" onclick="tFlash()"><svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg></div>
  <div class="restag" id="RT">VGA</div>
 </div>
 <div class="pn">
  <div id="noSD" class="warn" style="display:none" data-i="no_sd">No SD card</div>
  <div class="row3">
   <div class="stat"><b id="FC">0</b><small data-i="frames">frames</small></div>
   <div class="stat"><b id="EL">00:00</b><small data-i="elapsed">elapsed</small></div>
   <div class="stat"><b id="SF">--</b><small>MB free</small></div>
  </div>
  <div class="card">
   <div class="ctt" data-i="recording_ctrl">Recording</div>
   <div class="rw"><span class="lb" data-i="resolution">Resolution</span><div class="rc"><select id="sR" onchange="api('/set?res='+this.value)"></select></div></div>
   <div class="rw"><span class="lb" data-i="capture_rate">Capture rate</span><div class="rc"><input type="number" id="capVal" value="10" min="1" max="60" onchange="sendRate()"><span class="unit">FPS /</span><select id="capUnit" onchange="sendRate()"><option value="s">sec</option><option value="m">min</option><option value="h">hour</option></select></div></div>
   <div id="rateHint" class="hint" style="display:none"></div>
   <div class="rw"><span class="lb" data-i="duration">Duration</span><div class="rc"><input type="number" id="durVal" value="0" min="0" max="600" onchange="sendDur()"><span class="unit" data-i="min">min</span><span class="unit">(0=∞)</span></div></div>
  </div>
  <div class="btns">
   <button class="btn btn-p" id="bR" onclick="doRec(1)" data-i="start_rec">Start recording</button>
   <button class="btn btn-g" id="bS" onclick="doRec(0)" style="display:none" data-i="stop_rec">Stop recording</button>
  </div>
 </div>
</div>

<!-- ======== Files Tab ======== -->
<div class="pg" id="p-files">
 <div class="pn">
  <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px">
   <div class="ctt" style="margin:0" data-i="sd_files">SD card files</div>
   <button class="scb" onclick="toggleAll()" data-i="select_all">Select all</button>
  </div>
  <div id="fileActions" style="display:none;margin-bottom:8px">
   <div style="display:flex;gap:4px">
    <button class="btn btn-g" style="flex:1;font-size:11px;padding:6px" onclick="dlSelected()" data-i="dl_selected">Download</button>
    <button class="btn btn-g" style="flex:1;font-size:11px;padding:6px;color:var(--rd)" onclick="rmSelected()" data-i="del_selected">Delete</button>
   </div>
   <div style="font-size:10px;color:var(--t2);margin-top:4px" id="selCount"></div>
  </div>
  <div class="flist" id="FL"><div class="empty">Loading...</div></div>
 </div>
</div>

<!-- ======== Settings Tab ======== -->
<div class="pg" id="p-cfg">
 <div class="pn">

  <!-- WiFi (AP mode) -->
  <div id="cfAP">
   <div class="card">
    <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:10px">
     <div class="ctt" style="margin:0">WiFi</div><button class="scb" onclick="doScan()" data-i="scan">Scan</button></div>
    <div class="wlist" id="WL"><div class="empty" data-i="press_scan">Press Scan</div></div>
    <div class="fd"><div class="fl">SSID</div><input id="xS" placeholder="WiFi name"></div>
    <div class="fd"><div class="fl" data-i="password">Password</div><input id="xP" type="password" placeholder="WiFi password"></div>
   </div>
   <div class="card"><div class="ctt">IP</div>
    <div class="iptog"><button class="ipo on" onclick="tIP(0,this)">DHCP</button><button class="ipo" onclick="tIP(1,this)">Static</button></div>
    <div class="sip" id="SIF"><div class="fd"><div class="fl">IP</div><input id="xI" placeholder="192.168.4.2"></div>
    <div class="sr"><div class="fd"><div class="fl">Gateway</div><input id="xG" placeholder="192.168.4.1"></div><div class="fd"><div class="fl">Subnet</div><input id="xM" placeholder="255.255.255.0"></div></div></div>
   </div>
   <div class="btns"><button class="btn btn-p" style="flex:2" onclick="doWifi()" data-i="save_reboot">Save & reboot</button><button class="btn btn-g" style="flex:1;color:var(--rd)" onclick="doReset()" data-i="reset">Reset</button></div>
  </div>
  <!-- WiFi info (STA mode) -->
  <div id="cfSTA" style="display:none">
   <div class="card"><div class="ctt" data-i="network">Network</div>
    <div class="rw"><span class="lb">SSID</span><span class="vl" id="iSS">--</span></div>
    <div class="rw"><span class="lb">IP</span><span class="vl" id="iIP" style="color:var(--bl)">--</span></div>
    <div class="rw"><span class="lb" data-i="hostname">Hostname</span><span class="vl" id="iHN">--</span></div>
    <div class="rw"><span class="lb" data-i="signal">Signal</span><span class="vl" id="iRS">--</span></div>
   </div>
  </div>

  <!-- Section toggles (top) -->
  <div style="display:flex;flex-wrap:wrap;gap:6px;margin-bottom:12px">
   <button class="scb" style="padding:6px 14px;font-size:11px" onclick="tSec('camCard')" data-i="camera_settings">Camera</button>
   <button class="scb" style="padding:6px 14px;font-size:11px" onclick="tSec('recCard')" data-i="rec_settings">Recording</button>
   <button class="scb" style="padding:6px 14px;font-size:11px" onclick="tSec('logCard')" data-i="log_title">Log</button>
   <button class="scb" style="padding:6px 14px;font-size:11px" onclick="tSec('sysCard')" data-i="system">System</button>
  </div>

  <!-- Camera Settings -->
  <div class="card" id="camCard" style="display:none">
   <div class="ctt" data-i="camera_settings">Camera Settings</div>
   <div class="rw"><span class="lb" data-i="c_brightness">Brightness</span><div class="rc"><input type="range" min="-2" max="2" id="c_br" onchange="camSet('brightness',this.value);$('c_br_v').textContent=this.value"><span class="vl" id="c_br_v">1</span></div></div>
   <div class="tip" data-i="tip_brightness">Image brightness. Increase in dark environments</div>
   <div class="rw"><span class="lb" data-i="c_contrast">Contrast</span><div class="rc"><input type="range" min="-2" max="2" id="c_ct" onchange="camSet('contrast',this.value);$('c_ct_v').textContent=this.value"><span class="vl" id="c_ct_v">0</span></div></div>
   <div class="tip" data-i="tip_contrast">Light/dark difference. Higher = sharper, lower = softer</div>
   <div class="rw"><span class="lb" data-i="c_saturation">Saturation</span><div class="rc"><input type="range" min="-2" max="2" id="c_sa" onchange="camSet('saturation',this.value);$('c_sa_v').textContent=this.value"><span class="vl" id="c_sa_v">-1</span></div></div>
   <div class="tip" data-i="tip_saturation">Color intensity. 0 = natural colors</div>
   <div class="rw"><span class="lb" data-i="c_quality">Quality</span><div class="rc"><input type="range" min="4" max="63" id="c_q" onchange="camSet('quality',this.value);$('c_q_v').textContent=this.value"><span class="vl" id="c_q_v">20</span></div></div>
   <div class="tip" data-i="tip_quality">JPEG compression. Lower = better quality but larger file</div>
   <div class="rw"><span class="lb" data-i="c_gain">Gain ceiling</span><div class="rc"><select id="c_gc" onchange="camSet('gainceiling',this.value)"><option value="0">2x</option><option value="1">4x</option><option value="2" selected>8x</option><option value="3">16x</option><option value="4">32x</option><option value="5">64x</option><option value="6">128x</option></select></div></div>
   <div class="tip" data-i="tip_gain">Max brightness amplification. Higher = brighter in dark but more noise</div>
   <div class="rw"><span class="lb" data-i="c_wb">White balance</span><div class="rc"><select id="c_wb" onchange="camSet('wb',this.value)"><option value="0">Off</option><option value="1" selected>Auto</option></select></div></div>
   <div class="tip" data-i="tip_wb">Auto-adjusts color temperature to match lighting</div>
   <div class="rw"><span class="lb" data-i="c_hmirror">H-Mirror</span><div class="rc"><select id="c_hm" onchange="camSet('hmirror',this.value)"><option value="0">Off</option><option value="1">On</option></select></div></div>
   <div class="rw"><span class="lb" data-i="c_vflip">V-Flip</span><div class="rc"><select id="c_vf" onchange="camSet('vflip',this.value)"><option value="0">Off</option><option value="1">On</option></select></div></div>
   <div style="text-align:right;margin-top:8px"><button class="scb" onclick="camSet('reset','1');pollCam()" data-i="defaults">Defaults</button></div>
  </div>

  <!-- Recording Settings -->
  <div class="card" id="recCard" style="display:none">
   <div class="ctt" data-i="rec_settings">Recording Settings</div>
   <div class="rw"><span class="lb" data-i="playback_fps">Playback FPS</span><div class="rc"><input type="number" id="pbVal" value="15" min="1" max="60" onchange="api('/set?pbFps='+this.value)"><span class="unit">fps</span></div></div>
   <div class="tip" data-i="tip_pbfps">Video playback speed. 15~30fps recommended for timelapse</div>
   <div class="rw"><span class="lb" data-i="r_sdmin">SD min free</span><div class="rc"><input type="number" id="r_sd" value="10" min="1" max="1000" onchange="recSet('sdMinMB',this.value)"><span class="unit">MB</span></div></div>
   <div class="tip" data-i="tip_sdmin">Recording stops when free space drops below this</div>
   <div class="rw"><span class="lb" data-i="r_faillim">Fail limit</span><div class="rc"><input type="number" id="r_fl" value="5" min="1" max="100" onchange="recSet('failLimit',this.value)"></div></div>
   <div class="tip" data-i="tip_faillim">Auto-stop after this many consecutive capture failures</div>
   <div class="rw"><span class="lb" data-i="r_flush">Flush every</span><div class="rc"><input type="number" id="r_fe" value="10" min="0" max="1000" onchange="recSet('flushEvery',this.value)"><span class="unit" data-i="frames">frames</span></div></div>
   <div class="tip" data-i="tip_flush">Save file mid-recording every N frames. Protects against power loss</div>
   <div style="text-align:right;margin-top:8px"><button class="scb" onclick="recSet('reset','1');pollRec()" data-i="defaults">Defaults</button></div>
  </div>

  <!-- Log -->
  <div class="card" id="logCard" style="display:none">
   <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px">
    <div class="ctt" style="margin:0" data-i="log_title">Log</div>
    <div style="display:flex;gap:4px;align-items:center">
     <select id="logLvl" onchange="api('/loglevel?v='+this.value)" style="min-width:60px;font-size:10px"><option value="0">Min</option><option value="1" selected>Normal</option><option value="2">Verbose</option></select>
     <button class="scb" onclick="api('/logclear',function(){$('logBox').textContent='';toast('Log cleared')})" data-i="clear">Clear</button>
    </div>
   </div>
   <div class="logbox" id="logBox"></div>
  </div>

  <!-- System -->
  <div class="card" id="sysCard" style="display:none">
   <div class="ctt" data-i="system">System</div>
   <div class="fd"><div class="fl" data-i="device_name">Device name</div>
    <div style="display:flex;gap:4px"><input id="xH" value="femtocam" style="flex:1"><button class="btn btn-g" style="flex:0;padding:7px 12px;font-size:11px" onclick="saveName()" data-i="save">Save</button></div>
    <div class="tip" data-i="name_hint">Changes logo text and mDNS hostname. Requires reboot.</div></div>
   <div class="rw"><span class="lb" data-i="firmware">Firmware</span><span class="vl" id="iFW">--</span></div>
   <div class="rw"><span class="lb" data-i="uptime">Uptime</span><span class="vl" id="iUT">--</span></div>
   <div class="rw"><span class="lb">Heap</span><span class="vl" id="iHP">--</span></div>
   <div class="rw"><span class="lb">PSRAM</span><span class="vl" id="iPS">--</span></div>
   <div class="rw"><span class="lb">NTP</span><span class="vl" id="iNTP">--</span></div>
  </div>

  <button class="btn btn-g" style="width:100%;color:var(--rd);margin-top:8px" onclick="doReset()" data-i="factory_reset">Reset to factory</button>
 </div>
</div>

<div class="ft"><span>FEMTOCAM v2.3</span><span id="FI">--</span></div>
<div class="toast" id="toast"></div>

<script>
// ======== i18n ========
var L={en:{
  tab_cam:'Camera',tab_files:'Files',tab_cfg:'Settings',connecting:'Connecting...',stream_off:'Stream offline',
  no_sd:'No SD card \u2014 recording unavailable',frames:'frames',elapsed:'elapsed',
  recording_ctrl:'Recording',resolution:'Resolution',capture_rate:'Capture rate',
  playback_fps:'Playback FPS',tip_pbfps:'Video playback speed. 24~30fps for timelapse',
  duration:'Duration',min:'min',per_sec:'sec',per_min:'min',per_hour:'hour',
  start_rec:'Start recording',stop_rec:'Stop recording',
  sd_files:'SD card files',no_files:'No files',select_all:'Select all',
  dl_selected:'Download',del_selected:'Delete',selected:'selected',files_count:'files',renamed:'Renamed',
  scan:'Scan',press_scan:'Press Scan',no_networks:'No networks',password:'Password',
  save_reboot:'Save & reboot',reset:'Reset',factory_reset:'Reset to factory',
  network:'Network',hostname:'Hostname',signal:'Signal',device_name:'Device name',
  name_hint:'Changes logo text and mDNS hostname',save:'Save',saved_name:'Name saved',firmware:'Firmware',uptime:'Uptime',system:'System',
  camera_settings:'Camera',rec_settings:'Recording',log_title:'Log',clear:'Clear',defaults:'Defaults',
  c_brightness:'Brightness',c_contrast:'Contrast',c_saturation:'Saturation',c_quality:'Quality',
  c_gain:'Gain ceiling',c_wb:'White balance',c_hmirror:'H-Mirror',c_vflip:'V-Flip',
  tip_brightness:'Image brightness. Increase in dark',tip_contrast:'Light/dark difference',
  tip_saturation:'Color intensity. 0 = natural',tip_quality:'JPEG compression. Lower = better but larger',
  tip_gain:'Max brightness amplification. Higher = brighter but noisier',tip_wb:'Auto-adjust color to lighting',
  r_sdmin:'SD min free',r_faillim:'Fail limit',r_flush:'Flush every',
  tip_sdmin:'Stop recording when free space below this',tip_faillim:'Auto-stop after N consecutive failures',
  tip_flush:'Save mid-recording every N frames. Power-loss protection',
  rate_rec:'\u2605 recommended',rate_warn:'\u26A0 may affect streaming',
  rec_started:'Recording started',rec_saved:'Recording saved',
  confirm_wifi:'Save WiFi and reboot?',confirm_reset:'Reset all and reboot?',
  enter_ssid:'Enter SSID',deleted:'Deleted',confirm_del:'Delete ',
},ko:{
  tab_cam:'\uCE74\uBA54\uB77C',tab_files:'\uD30C\uC77C',tab_cfg:'\uC124\uC815',
  connecting:'\uC5F0\uACB0 \uC911...',stream_off:'\uC2A4\uD2B8\uB9BC \uC624\uD504\uB77C\uC778',
  no_sd:'SD \uCE74\uB4DC \uC5C6\uC74C \u2014 \uB179\uD654 \uBD88\uAC00',frames:'\uD504\uB808\uC784',elapsed:'\uACBD\uACFC',
  recording_ctrl:'\uB179\uD654',resolution:'\uD574\uC0C1\uB3C4',capture_rate:'\uCEA1\uCC98 \uC18D\uB3C4',
  playback_fps:'\uC7AC\uC0DD FPS',tip_pbfps:'\uB3D9\uC601\uC0C1 \uC7AC\uC0DD \uC18D\uB3C4. \uD0C0\uC784\uB7A9\uC2A4\uB294 24~30fps \uAD8C\uC7A5',
  duration:'\uB179\uD654 \uC2DC\uAC04',min:'\uBD84',per_sec:'\uCD08\uB2F9',per_min:'\uBD84\uB2F9',per_hour:'\uC2DC\uAC04\uB2F9',
  start_rec:'\uB179\uD654 \uC2DC\uC791',stop_rec:'\uB179\uD654 \uC815\uC9C0',
  sd_files:'SD \uD30C\uC77C',no_files:'\uD30C\uC77C \uC5C6\uC74C',select_all:'\uC804\uCCB4 \uC120\uD0DD',
  dl_selected:'\uB2E4\uC6B4\uB85C\uB4DC',del_selected:'\uC0AD\uC81C',selected:'\uC120\uD0DD',files_count:'\uAC1C',renamed:'\uC774\uB984 \uBCC0\uACBD',
  scan:'\uC2A4\uCE94',press_scan:'\uC2A4\uCE94 \uB204\uB974\uC138\uC694',no_networks:'\uB124\uD2B8\uC6CC\uD06C \uC5C6\uC74C',password:'\uBE44\uBC00\uBC88\uD638',
  save_reboot:'\uC800\uC7A5 \uBC0F \uC7AC\uBD80\uD305',reset:'\uCD08\uAE30\uD654',factory_reset:'\uACF5\uC7A5 \uCD08\uAE30\uD654',
  network:'\uB124\uD2B8\uC6CC\uD06C',hostname:'\uD638\uC2A4\uD2B8\uBA85',signal:'\uC2E0\uD638',
  device_name:'\uAE30\uAE30 \uC774\uB984',name_hint:'\uB85C\uACE0\uC640 mDNS\uC5D0 \uBC18\uC601',save:'\uC800\uC7A5',saved_name:'\uC774\uB984 \uC800\uC7A5\uB428',
  firmware:'\uD380\uC6E8\uC5B4',uptime:'\uAC00\uB3D9\uC2DC\uAC04',system:'\uC2DC\uC2A4\uD15C',
  camera_settings:'\uCE74\uBA54\uB77C',rec_settings:'\uB179\uD654',log_title:'\uB85C\uADF8',clear:'\uC9C0\uC6B0\uAE30',defaults:'\uAE30\uBCF8\uAC12',
  c_brightness:'\uBC1D\uAE30',c_contrast:'\uBA85\uC554',c_saturation:'\uCC44\uB3C4',c_quality:'\uD654\uC9C8',
  c_gain:'\uAC10\uB3C4 \uC99D\uD3ED',c_wb:'\uD654\uC774\uD2B8\uBC38\uB7F0\uC2A4',c_hmirror:'\uC88C\uC6B0\uBC18\uC804',c_vflip:'\uC0C1\uD558\uBC18\uC804',
  tip_brightness:'\uC774\uBBF8\uC9C0 \uBC1D\uAE30. \uC5B4\uB450\uC6B4 \uD658\uACBD\uC5D0\uC11C \uB192\uC774\uC138\uC694',
  tip_contrast:'\uBA85\uC554 \uB300\uBE44. \uB192\uC774\uBA74 \uC120\uBA85, \uB0AE\uCD94\uBA74 \uBD80\uB4DC\uB7EC\uC6C0',
  tip_saturation:'\uC0C9 \uC9C4\uD558\uAE30. 0=\uC790\uC5F0\uC2A4\uB7EC\uC6B4 \uC0C9\uAC10',
  tip_quality:'JPEG \uC555\uCD95. \uB0AE\uC744\uC218\uB85D \uACE0\uD654\uC9C8(\uD30C\uC77C \uCEE4\uC9D0)',
  tip_gain:'\uCD5C\uB300 \uBC1D\uAE30 \uC99D\uD3ED. \uB192\uC774\uBA74 \uBC1D\uC544\uC9C0\uC9C0\uB9CC \uB178\uC774\uC988 \uC99D\uAC00',
  tip_wb:'\uC870\uBA85\uC0C9\uC5D0 \uB9DE\uCDB0 \uC790\uB3D9 \uC0C9\uC628\uB3C4 \uC870\uC815',
  r_sdmin:'SD \uCD5C\uC18C \uC5EC\uC720',r_faillim:'\uC2E4\uD328 \uD55C\uB3C4',r_flush:'\uC800\uC7A5 \uC8FC\uAE30',
  tip_sdmin:'\uC5EC\uC720 \uC6A9\uB7C9\uC774 \uC774\uD558\uBA74 \uB179\uD654 \uC790\uB3D9 \uC815\uC9C0',
  tip_faillim:'\uC5F0\uC18D N\uD68C \uCEA1\uCC98 \uC2E4\uD328 \uC2DC \uC790\uB3D9 \uC815\uC9C0',
  tip_flush:'N\uD504\uB808\uC784\uB9C8\uB2E4 \uC911\uAC04 \uC800\uC7A5. \uC804\uC6D0 \uCC28\uB2E8 \uBCF4\uD638',
  rate_rec:'\u2605 \uAD8C\uC7A5',rate_warn:'\u26A0 \uC2A4\uD2B8\uB9AC\uBC0D \uC601\uD5A5',
  rec_started:'\uB179\uD654 \uC2DC\uC791',rec_saved:'\uB179\uD654 \uC800\uC7A5',
  confirm_wifi:'WiFi \uC800\uC7A5 \uD6C4 \uC7AC\uBD80\uD305?',confirm_reset:'\uC804\uCCB4 \uCD08\uAE30\uD654 \uD6C4 \uC7AC\uBD80\uD305?',
  enter_ssid:'SSID \uC785\uB825',deleted:'\uC0AD\uC81C\uB428',confirm_del:'\uC0AD\uC81C: ',
}};
var lang='en';
function t(k){return L[lang][k]||L.en[k]||k}
function applyLang(){document.querySelectorAll('[data-i]').forEach(function(el){var k=el.getAttribute('data-i');if(t(k))el.textContent=t(k)});$('langBtn').textContent=lang==='en'?'KO':'EN';buildSelects()}
function toggleLang(){lang=lang==='en'?'ko':'en';applyLang();api('/set?lang='+(lang==='en'?0:1))}

// ======== Core ========
var B='',rec=false,fl=false;
function $(i){return document.getElementById(i)}
function api(p,cb){fetch(p).then(function(r){return r.text()}).then(function(r){if(cb)cb(r);if(r.indexOf('ERR:')===0)toast(r.substr(4),1)}).catch(function(){toast('Connection error',1)})}
function apiJ(p,cb){fetch(p).then(function(r){return r.json()}).then(cb).catch(function(){})}
function toast(m,e){var o=$('toast');o.textContent=m;o.className='toast'+(e?' err':'')+' show';setTimeout(function(){o.classList.remove('show')},2500)}
function pad(n){return String(n).padStart(2,'0')}
function syncSel(s,v){for(var i=0;i<s.options.length;i++)if(s.options[i].value===String(v)){s.selectedIndex=i;break}}
function T(n){document.querySelectorAll('.tab').forEach(function(t,i){t.classList.toggle('on',['cam','files','cfg'][i]===n)});document.querySelectorAll('.pg').forEach(function(p){p.classList.remove('on')});$('p-'+n).classList.add('on');if(n==='files')loadFiles();if(n==='cfg'){pollCam();pollRec();pollSys();pollLog()}}
function tSec(id){var el=$(id);el.style.display=el.style.display==='none'?'block':'none'}
function buildSelects(){var sr=$('sR');var sv=sr.value;sr.innerHTML='';['QVGA','VGA','SVGA'].forEach(function(r){var o=document.createElement('option');o.value=r;o.text=r;sr.add(o)});if(sv)sr.value=sv;var cu=$('capUnit');cu.options[0].text=t('per_sec');cu.options[1].text=t('per_min');cu.options[2].text=t('per_hour')}

// ======== Logo ========
function updateLogo(name){var n=(name||'femtocam');n=n.charAt(0).toUpperCase()+n.slice(1);var h=n.replace(/cam/i,function(m){return'<b>'+m+'</b>'});if(h.indexOf('<b>')<0)h=n;$('logoName').innerHTML=h}

// ======== Stream ========
var streamOK=false,streamRetry=0;
function startStream(){var img=$('S'),err=$('SE');var u=location.protocol+'//'+location.hostname+':81/stream?t='+Date.now();err.textContent=t('connecting');err.style.display='flex';img.style.display='none';img.src=u;img.onload=function(){img.style.display='block';err.style.display='none';streamOK=true;streamRetry=0};img.onerror=function(){img.style.display='none';err.style.display='flex';streamOK=false;streamRetry++;var d=Math.min(streamRetry*2000,10000);err.textContent=t('stream_off')+' ('+Math.round(d/1000)+'s)';setTimeout(startStream,d)}}
function tFlash(){fl=!fl;api('/flash?on='+(fl?1:0),function(){$('FB').classList.toggle('on',fl)})}

// ======== Recording ========
function doRec(s){api(s?'/start':'/stop',function(r){if(r==='OK')toast(s?t('rec_started'):t('rec_saved'))})}
function sendRate(){var fps=parseInt($('capVal').value)||1,u=$('capUnit').value;var ms;if(u==='s')ms=Math.round(1000/fps);else if(u==='m')ms=Math.round(60000/fps);else ms=Math.round(3600000/fps);ms=Math.max(42,Math.min(3600000,ms));api('/set?capMs='+ms);var h=$('rateHint');if(u==='s'&&fps>15){h.textContent=t('rate_warn');h.style.display='block';h.style.color='var(--yl)'}else if(u==='s'&&fps>=10){h.textContent=t('rate_rec');h.style.display='block';h.style.color='var(--gn)'}else h.style.display='none'}
function capMsToUI(ms){if(ms<=1000){$('capVal').value=Math.round(1000/ms);$('capUnit').value='s'}else if(ms<=60000){$('capVal').value=Math.max(1,Math.round(60000/ms));$('capUnit').value='m'}else{$('capVal').value=Math.max(1,Math.round(3600000/ms));$('capUnit').value='h'}}
function sendDur(){var v=parseInt($('durVal').value);if(isNaN(v)||v<0)v=0;if(v>600)v=600;api('/set?dur='+v)}

// ======== Status polling ========
var pollIv=null;
function startPoll(){if(pollIv)clearInterval(pollIv);pollIv=setInterval(poll,rec?2000:5000)}
function poll(){apiJ('/status',function(d){
  if(d.hostname)updateLogo(d.hostname);
  var ap=d.mode==='AP';$('M').className='mb '+(ap?'mb-a':'mb-s');$('M').textContent=d.mode;$('MI').textContent=d.ip;$('FI').textContent=ap?'AP: FEMTOCAM':d.hostname+'.local';
  $('cfAP').style.display=ap?'block':'none';$('cfSTA').style.display=ap?'none':'block';
  if(!ap){$('iSS').textContent=d.ssid;$('iIP').textContent=d.ip;$('iHN').textContent=d.hostname+'.local';$('iRS').textContent=d.rssi+' dBm'}
  $('RT').textContent=d.resolution;$('FC').textContent=d.frameCount;$('SF').textContent=d.sdReady?d.freeSpace:'--';
  $('noSD').style.display=d.sdReady?'none':'block';$('bR').disabled=!d.sdReady;
  if(d.recording){var s=d.recElapsed||0;$('EL').textContent=pad(Math.floor(s/60))+':'+pad(s%60)}
  if(d.recording&&!rec){rec=true;$('bR').style.display='none';$('bS').style.display='block';$('RB').classList.add('on');$('sR').disabled=true;$('capVal').disabled=true;$('capUnit').disabled=true;$('pbVal').disabled=true;$('durVal').disabled=true;startPoll()}
  if(!d.recording&&rec){rec=false;$('bR').style.display='block';$('bS').style.display='none';$('RB').classList.remove('on');$('sR').disabled=false;$('capVal').disabled=false;$('capUnit').disabled=false;$('pbVal').disabled=false;$('durVal').disabled=false;$('EL').textContent='00:00';startPoll()}
  fl=d.flash;$('FB').classList.toggle('on',fl);syncSel($('sR'),d.resolution);
  if(d.capMs!==undefined)capMsToUI(d.capMs);if(d.pbFps!==undefined)$('pbVal').value=d.pbFps;if(d.duration!==undefined)$('durVal').value=d.duration;
  if(d.lang!==undefined&&d.lang!==(lang==='en'?0:1)){lang=d.lang===1?'ko':'en';applyLang()}
})}

// ======== Camera settings ========
function camSet(k,v){api('/camset?'+k+'='+v)}
function pollCam(){apiJ('/cam',function(d){$('c_br').value=d.brightness;$('c_br_v').textContent=d.brightness;$('c_ct').value=d.contrast;$('c_ct_v').textContent=d.contrast;$('c_sa').value=d.saturation;$('c_sa_v').textContent=d.saturation;$('c_q').value=d.quality;$('c_q_v').textContent=d.quality;$('c_gc').value=d.gainceiling;$('c_wb').value=d.wb;$('c_hm').value=d.hmirror;$('c_vf').value=d.vflip})}

// ======== Recording params ========
function recSet(k,v){api('/recset?'+k+'='+v)}
function pollRec(){apiJ('/recparams',function(d){$('r_sd').value=d.sdMinMB;$('r_fl').value=d.failLimit;$('r_fe').value=d.flushEvery})}

// ======== System ========
function pollSys(){apiJ('/sysinfo',function(d){$('iFW').textContent=d.firmware;$('iUT').textContent=d.uptime;$('iHP').textContent=Math.round(d.freeHeap/1024)+' KB';if(d.psram)$('iPS').textContent=Math.round(d.psramFree/1024)+' KB';$('iNTP').textContent=d.ntpOK?'Synced':'Not available'})}

// ======== Log ========
function pollLog(){apiJ('/log',function(lines){var box=$('logBox');box.textContent=lines.join('\n');box.scrollTop=box.scrollHeight})}
setInterval(function(){if($('logCard').style.display!=='none')pollLog()},3000);

// ======== Files ========
var fileList=[];
function loadFiles(){apiJ('/files',function(files){fileList=files||[];var el=$('FL');if(!fileList.length){el.innerHTML='<div class="empty">'+t('no_files')+'</div>';updFA();return}var h='';fileList.forEach(function(f,i){var ir=f.recording;h+='<div class="fitem"><input type="checkbox" class="fchk" data-idx="'+i+'" onchange="updFA()"'+(ir?' disabled':'')+'/><div class="finfo"><span class="fname" id="fn_'+i+'">'+esc(f.name)+'</span>';if(ir)h+='<span class="frec">REC</span>';h+='<div class="fsize">'+f.size+'</div></div><div class="fbtns">';if(!ir)h+='<button onclick="doDl(\''+esc(f.name)+'\')">DL</button><button class="ren" onclick="doRen('+i+')">REN</button><button class="del" onclick="doDel(\''+esc(f.name)+'\')">DEL</button>';h+='</div></div>'});el.innerHTML=h;updFA()})}
function esc(s){return s.replace(/'/g,"\\'").replace(/"/g,'&quot;')}
function getChk(){var r=[];document.querySelectorAll('.fchk:checked').forEach(function(c){r.push(parseInt(c.getAttribute('data-idx')))});return r}
function updFA(){var s=getChk();$('fileActions').style.display=s.length?'block':'none';$('selCount').textContent=s.length+' '+t('selected')}
function toggleAll(){var b=document.querySelectorAll('.fchk:not(:disabled)');var all=true;b.forEach(function(x){if(!x.checked)all=false});b.forEach(function(x){x.checked=!all});updFA()}
function doDl(n){window.location.href='/dl?f='+encodeURIComponent(n)}
function doDel(n){if(!confirm(t('confirm_del')+n+'?'))return;api('/rm?f='+encodeURIComponent(n),function(r){if(r==='OK'){toast(t('deleted'));loadFiles()}})}
function dlSelected(){var s=getChk();if(!s.length)return;var i=0;function nx(){if(i>=s.length)return;doDl(fileList[s[i]].name);i++;setTimeout(nx,800)}nx()}
function rmSelected(){var s=getChk();if(!s.length)return;var nm=s.map(function(i){return fileList[i].name});if(!confirm(t('confirm_del')+nm.length+' '+t('files_count')+'?'))return;var d=0;nm.forEach(function(n){api('/rm?f='+encodeURIComponent(n),function(){d++;if(d>=nm.length){toast(d+' '+t('deleted'));loadFiles()}})})}
function doRen(i){var f=fileList[i],el=$('fn_'+i);var b=f.name.replace(/^\//,'').replace(/\.avi$/i,'');el.innerHTML='<input class="frename" id="ri_'+i+'" value="'+b+'" onkeydown="if(event.key===\'Enter\')cfRen('+i+')" onblur="cfRen('+i+')">';$('ri_'+i).focus();$('ri_'+i).select()}
function cfRen(i){var inp=$('ri_'+i);if(!inp)return;var nn=inp.value.trim();if(!nn){loadFiles();return}if(!nn.toLowerCase().endsWith('.avi'))nn+='.avi';var on=fileList[i].name;if(nn===on.replace(/^\//,'')){loadFiles();return}api('/rename?f='+encodeURIComponent(on)+'&n='+encodeURIComponent(nn),function(r){if(r==='OK'){toast(t('renamed'));loadFiles()}})}

// ======== WiFi ========
function doScan(){var btn=event.target;btn.textContent='...';btn.disabled=true;apiJ('/scan',function(list){btn.textContent=t('scan');btn.disabled=false;if(!list||!list.length){$('WL').innerHTML='<div class="empty">'+t('no_networks')+'</div>';return}var h='';list.forEach(function(a){var e=a.ssid.replace(/&/g,'&amp;').replace(/</g,'&lt;');h+='<div class="witem" onclick="selW(this,\''+a.ssid.replace(/'/g,"\\'")+'\')">';h+='<span style="font-size:12px">'+e+(a.enc?'':'<span style="color:var(--gn);margin-left:4px;font-size:9px">OPEN</span>')+'</span><span class="wrssi">'+a.rssi+' dBm</span></div>'});$('WL').innerHTML=h})}
function selW(el,s){document.querySelectorAll('.witem').forEach(function(w){w.classList.remove('sel')});el.classList.add('sel');$('xS').value=s}
function tIP(m,el){document.querySelectorAll('.ipo').forEach(function(o){o.classList.remove('on')});el.classList.add('on');$('SIF').classList.toggle('sh',m===1)}
function doWifi(){var s=$('xS').value;if(!s){toast(t('enter_ssid'),1);return}if(!confirm(t('confirm_wifi')))return;var q='/wifi?ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent($('xP').value)+'&hostname='+encodeURIComponent($('xH').value)+'&ip_mode='+(document.querySelector('.ipo:last-child').classList.contains('on')?1:0);if(document.querySelector('.ipo:last-child').classList.contains('on'))q+='&ip='+$('xI').value+'&gw='+$('xG').value+'&sn='+$('xM').value;api(q,function(){toast('Rebooting...');setTimeout(function(){location.reload()},5000)})}
function doReset(){if(!confirm(t('confirm_reset')))return;api('/reset',function(){toast('Resetting...');setTimeout(function(){location.reload()},5000)})}
function saveName(){var n=$('xH').value.trim();if(!n){toast('Enter a name',1);return}updateLogo(n);api('/set?hostname='+encodeURIComponent(n),function(r){if(r==='OK')toast(t('saved_name'))})}

// ======== Init ========
window.onload=function(){B=location.protocol+'//'+location.hostname;applyLang();startStream();poll();startPoll()};
</script>
</body></html>
)rawliteral";
