#include <EtherCard.h>

#define BUTTON_PIN 2

static byte myip[] = {169, 254, 238, 217};
static byte gwip[] = {169, 254, 238, 215};
static byte serverip[] = {169, 254, 238, 216};
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x32};
byte Ethernet::buffer[500];

bool lastButton = HIGH;
unsigned long lastSend = 0;
byte session = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  ether.begin(sizeof Ethernet::buffer, mymac, SS);
  ether.staticSetup(myip, gwip);
  
  Serial.println(F("Client started"));
  Serial.print(F("IP: "));
  Serial.print(myip[0]); Serial.print('.');
  Serial.print(myip[1]); Serial.print('.');
  Serial.print(myip[2]); Serial.print('.');
  Serial.println(myip[3]);
}

void sendRequest(const char* path) {
  Serial.print(F("Sending request to: "));
  Serial.println(path);
  
  ether.browseUrl(PSTR(""), path, serverip, clientCallback);
}

static void clientCallback(byte status, word off, word len) {
  Serial.print(F("Response status: "));
  Serial.println(status);
  
  if (status == 0 && len > 0) {
    Serial.print(F("Data: "));
    char* data = (char*)Ethernet::buffer + off;
    for (word i = 0; i < len; i++) {
      Serial.print(data[i]);
    }
    Serial.println();
  }
}

void loop() {
  ether.packetLoop(ether.packetReceive());
  
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButton == HIGH) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      sendRequest("/toggle");
      lastButton = LOW;
    }
  } else if (buttonState == HIGH) {
    lastButton = HIGH;
  }

  if (millis() - lastSend >= 5000) {
    sendRequest("/count");
    lastSend = millis();
  }
}