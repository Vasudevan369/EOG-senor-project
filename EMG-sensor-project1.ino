#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

/* ================= WIFI ================= */
const char* WIFI_SSID = "IBM";
const char* WIFI_PASS = "Vasu@2004";

/* ================= HARDWARE ================= */
const int ADC_PIN = 34;
const int ADC_BITS = 12;

/* ================= SERVERS ================= */
WebServer server(80);
WebSocketsServer ws(81);

/* ================= EOG SIGNAL ================= */
float baseline = 0;
float noiseLevel = 0;

/* Thresholds */
int MOVE_THRESHOLD = 0;
int BLINK_THRESHOLD = 0;

/* Timing */
unsigned long lastActionTime = 0;
const unsigned long ACTION_COOLDOWN_MS = 500;
const unsigned long CONFIRM_TIME_MS = 120;

/* ================= HTML PAGE ================= */
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>EOG Emergency Communication</title>

<style>
body {
  background:black;
  margin:0;
  overflow:hidden;
  font-family:system-ui;
  color:white;
}

/* BIG CURSOR */
#cursor {
  position:absolute;
  width:70px;
  height:70px;
  border:6px solid white;
  border-radius:50%;
  transform:translate(-50%,-50%);
  box-shadow:0 0 25px white;
  pointer-events:none;
  transition:0.2s;
}

/* GRID */
.grid {
  display:grid;
  grid-template-columns:repeat(2,1fr);
  gap:25px;
  max-width:440px;
  margin:80px auto;
}

.card {
  background:#1a1a1a;
  border-radius:22px;
  padding:35px;
  text-align:center;
  font-size:30px;
  border:5px solid transparent;
}

.card.active {
  border-color:#00ffff;
  box-shadow:0 0 30px #00ffff;
}

/* FULL SCREEN */
.fullscreen {
  position:fixed;
  inset:0;
  background:black;
  display:flex;
  align-items:center;
  justify-content:center;
  font-size:70px;
  z-index:10;
}

.hidden { display:none; }

/* CALIBRATION PANEL */
#calib {
  position:fixed;
  top:10px;
  left:10px;
  background:#111;
  padding:10px 14px;
  border-radius:10px;
  font-size:12px;
  color:#9f9;
  box-shadow:0 0 10px rgba(0,255,0,0.3);
}
</style>
</head>

<body>

<div id="calib">
  <div>Baseline: <span id="b">-</span></div>
  <div>Noise: <span id="n">-</span></div>
  <div>Move Th: <span id="m">-</span></div>
  <div>Blink Th: <span id="bl">-</span></div>
  <div>Live v: <span id="v">-</span></div>
</div>

<div id="cursor"></div>

<div class="grid">
  <div class="card">💧<br>Water</div>
  <div class="card">❤️<br>Heart Pain</div>
  <div class="card">😴<br>Sleepy</div>
  <div class="card">🚨<br>Emergency</div>
</div>

<div id="big" class="fullscreen hidden"></div>

<script>
let ws;
let index = 0;
const cards = document.querySelectorAll(".card");
const cursor = document.getElementById("cursor");
const big = document.getElementById("big");

let pos = [];

function calcPos() {
  pos = [];
  cards.forEach(c=>{
    const r=c.getBoundingClientRect();
    pos.push({x:r.left+r.width/2,y:r.top+r.height/2});
  });
}

function updateCursor() {
  cards.forEach(c=>c.classList.remove("active"));
  cards[index].classList.add("active");
  cursor.style.left = pos[index].x+"px";
  cursor.style.top  = pos[index].y+"px";
}

function selectCard() {
  big.innerHTML = cards[index].innerHTML;
  big.classList.remove("hidden");
  setTimeout(()=>big.classList.add("hidden"),3000);
}

function connectWS() {
  ws = new WebSocket("ws://"+location.hostname+":81/");
  ws.onmessage = e=>{
    const d = JSON.parse(e.data);

    if(d.type === "action") {
      if(d.val === "next") {
        index = (index+1) % cards.length;
        updateCursor();
      }
      if(d.val === "select") selectCard();
    }

    if(d.type === "calib") {
      document.getElementById("b").innerText  = d.base.toFixed(1);
      document.getElementById("n").innerText  = d.noise.toFixed(1);
      document.getElementById("m").innerText  = d.move;
      document.getElementById("bl").innerText = d.blink;
      document.getElementById("v").innerText  = d.v;
    }
  };
}

window.onload=()=>{
  calcPos();
  updateCursor();
  connectWS();
};
window.onresize=calcPos;
</script>

</body>
</html>
)rawliteral";

/* ================= HELPERS ================= */
void sendAction(const char* a) {
  ws.broadcastTXT(String("{\"type\":\"action\",\"val\":\"") + a + "\"}");
}

void sendCalib(float v) {
  String msg = String("{\"type\":\"calib\",") +
               "\"base\":" + baseline +
               ",\"noise\":" + noiseLevel +
               ",\"move\":" + MOVE_THRESHOLD +
               ",\"blink\":" + BLINK_THRESHOLD +
               ",\"v\":" + v + "}";
  ws.broadcastTXT(msg);
}

void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

/* ================= CALIBRATION ================= */
void calibrateEOG() {
  delay(3000);

  float sum = 0, sumSq = 0;
  const int N = 400;

  for(int i=0;i<N;i++){
    int v = analogRead(ADC_PIN);
    sum += v;
    sumSq += v*v;
    delay(5);
  }

  baseline = sum / N;
  noiseLevel = sqrt(sumSq / N - baseline * baseline);

  /* 🔥 EASIER MOVEMENT */
  MOVE_THRESHOLD  = noiseLevel * 1.5;
  BLINK_THRESHOLD = noiseLevel * 3.0;

  if (MOVE_THRESHOLD < 30) MOVE_THRESHOLD = 30;
  if (BLINK_THRESHOLD < 90) BLINK_THRESHOLD = 90;
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  analogReadResolution(ADC_BITS);
  pinMode(ADC_PIN, INPUT);

  calibrateEOG();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status()!=WL_CONNECTED) delay(300);

  server.on("/", handleRoot);
  server.begin();
  ws.begin();
}

/* ================= LOOP ================= */
void loop() {
  server.handleClient();
  ws.loop();

  static unsigned long aboveStart = 0;

  int raw = analogRead(ADC_PIN);
  int v = abs(raw - baseline);
  unsigned long now = millis();

  sendCalib(v);

  if(now - lastActionTime < ACTION_COOLDOWN_MS) return;

  if(v > MOVE_THRESHOLD) {
    if(aboveStart == 0) aboveStart = now;

    if(now - aboveStart > CONFIRM_TIME_MS) {
      if(v > BLINK_THRESHOLD) sendAction("select");
      else sendAction("next");

      lastActionTime = now;
      aboveStart = 0;
    }
  } else {
    aboveStart = 0;
  }

  delay(40);
}
