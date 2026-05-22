#include <EtherCard.h>

#define RED_PIN 6
#define YELLOW_PIN 7
#define GREEN_PIN 8

static byte myip[] = {169, 254, 238, 216};
static byte gwip[] = {169, 254, 238, 215};
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
byte Ethernet::buffer[500];

enum TrafficState { RED, RED_YELLOW, GREEN, YELLOW, BLINK_YELLOW };

TrafficState state = RED;
bool autoMode = false;
unsigned long lastChange = 0;
unsigned long timer = 0;
bool blinkState = false;
unsigned long lastBlink = 0;

void setState(TrafficState s) {
  state = s;
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  
  switch(s) {
    case RED:
      digitalWrite(RED_PIN, HIGH);
      break;
    case RED_YELLOW:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, HIGH);
      break;
    case GREEN:
      digitalWrite(GREEN_PIN, HIGH);
      break;
    case YELLOW:
      digitalWrite(YELLOW_PIN, HIGH);
      break;
    case BLINK_YELLOW:
      break;
  }
}

void autoNext() {
  switch(state) {
    case RED:
      setState(RED_YELLOW);
      timer = 250;
      break;
    case RED_YELLOW:
      setState(GREEN);
      timer = 750;
      break;
    case GREEN:
      setState(YELLOW);
      timer = 500;
      break;
    case YELLOW:
      setState(BLINK_YELLOW);
      timer = 500;
      blinkState = false;
      lastBlink = millis();
      break;
    case BLINK_YELLOW:
      setState(RED);
      timer = 750;
      break;
  }
  lastChange = millis();
}

const char page1[] PROGMEM = 
  "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
  "<!DOCTYPE html><html><head><title>Traffic</title><style>"
  "body{font-family:Arial;text-align:center;margin:50px;background:#f0f0f0}"
  "h1{color:#333}.btn{display:inline-block;padding:15px 30px;margin:10px;"
  "font-size:18px;border:none;border-radius:5px;color:white;text-decoration:none}"
  ".r{background:#f00}.y{background:#ff0;color:#000}.g{background:#0f0;color:#000}"
  ".o{background:#666}.a{background:#06f}.s{margin:30px;padding:20px;"
  "background:white;border-radius:10px;font-size:24px}</style></head><body>"
  "<h1>Traffic Light</h1><div class='s'>Status: <b>";

const char page2[] PROGMEM = 
  "</b></div><a href='/red' class='btn r'>RED</a>"
  "<a href='/yellow' class='btn y'>YELLOW</a>"
  "<a href='/green' class='btn g'>GREEN</a><br>"
  "<a href='/off' class='btn o'>OFF</a>"
  "<a href='/auto' class='btn a'>AUTO</a></body></html>";

void sendPage() {
  BufferFiller bfill = ether.tcpOffset();
  
  char buf[60];
  strcpy_P(buf, page1);
  bfill.emit_raw(buf, strlen(buf));
  
  const char* status = "?";
  if (autoMode) status = "AUTO";
  else {
    switch(state) {
      case RED: status = "RED"; break;
      case RED_YELLOW: status = "RED+YELLOW"; break;
      case YELLOW: status = "YELLOW"; break;
      case GREEN: status = "GREEN"; break;
      case BLINK_YELLOW: status = "BLINK"; break;
    }
  }
  bfill.emit_raw(status, strlen(status));
  
  strcpy_P(buf, page2);
  bfill.emit_raw(buf, strlen(buf));
  
  ether.httpServerReply(bfill.position());
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  setState(RED);
  
  ether.begin(sizeof Ethernet::buffer, mymac, SS);

  ether.staticSetup(myip, gwip);
  Serial.println(F("Started"));
}

void loop() {
  word pos = ether.packetLoop(ether.packetReceive());
  if (pos) {
    char* data = (char*)Ethernet::buffer + pos;
    
    if (strstr(data, "GET /red")) {
      autoMode = false;
      setState(RED);
    } else if (strstr(data, "GET /yellow")) {
      autoMode = false;
      setState(YELLOW);
    } else if (strstr(data, "GET /green")) {
      autoMode = false;
      setState(GREEN);
    } else if (strstr(data, "GET /off")) {
      autoMode = false;
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
    } else if (strstr(data, "GET /auto")) {
      autoMode = true;
      setState(RED);
      timer = 5000;
      lastChange = millis();
    }
    sendPage();
  }
  
  if (autoMode) {
    if (state == BLINK_YELLOW) {
      if (millis() - lastBlink >= 500) {
        blinkState = !blinkState;
        digitalWrite(YELLOW_PIN, blinkState);
        lastBlink = millis();
      }
    }
    
    if (millis() - lastChange >= timer) {
      autoNext();
    }
  }
}