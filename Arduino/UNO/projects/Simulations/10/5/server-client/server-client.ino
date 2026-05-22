#include <EtherCard.h>

#define LED_PIN 9

static byte myip[] = {169, 254, 238, 216};
static byte gwip[] = {169, 254, 238, 215};
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
byte Ethernet::buffer[500];

int counter = 0;

const char page[] PROGMEM = 
  "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n";

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  
  ether.begin(sizeof Ethernet::buffer, mymac, SS);
  ether.staticSetup(myip, gwip);
  
  Serial.println(F("Server started"));
  Serial.print(F("IP: "));
  Serial.print(myip[0]); Serial.print('.');
  Serial.print(myip[1]); Serial.print('.');
  Serial.print(myip[2]); Serial.print('.');
  Serial.println(myip[3]);
}

void loop() {
  word pos = ether.packetLoop(ether.packetReceive());
  
  if (pos) {
    char* data = (char*)Ethernet::buffer + pos;
    
    Serial.print(F("Received: "));
    Serial.println(data);
    
    if (strstr(data, "GET /toggle")) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      Serial.println(F("LED toggled"));
    }
    
    if (strstr(data, "GET /count")) {
      counter++;
      Serial.print(F("Counter: "));
      Serial.println(counter);
    }
    
    BufferFiller bfill = ether.tcpOffset();
    char buf[40];
    strcpy_P(buf, page);
    bfill.emit_raw(buf, strlen(buf));
    
    sprintf(buf, "Counter: %d\nLED: %d\n", counter, digitalRead(LED_PIN));
    bfill.emit_raw(buf, strlen(buf));
    
    ether.httpServerReply(bfill.position());
  }
}