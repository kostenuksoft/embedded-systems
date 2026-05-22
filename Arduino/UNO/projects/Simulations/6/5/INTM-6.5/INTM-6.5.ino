#include <avr/wdt.h>
#include <avr/interrupt.h>

class WDTDemo {
private:
  const byte ledPin = 13;
  const byte lcdRS = 7;
  const byte lcdEN = 9;
  const byte lcdD4 = 10;
  const byte lcdD5 = 11;
  const byte lcdD6 = 12;
  const byte lcdD7 = 8;
  const byte buttonPin = 2;
  
  volatile uint32_t wdtCounter;
  volatile bool ledState;
  
  byte currentMode;
  bool lastButtonState;
  unsigned long lastLCDUpdate;
  
  const char* modeNames[4] = {
    "1s  ",
    "500ms",
    "250ms",
    "120ms"
  };

public:
  WDTDemo() : 
    wdtCounter(0), 
    ledState(false), 
    currentMode(0),
    lastButtonState(HIGH),
    lastLCDUpdate(0) {}
  
  void begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    
    wdt_disable();
    
    initLCD();
    setupWDT();
    updateDisplay();
  }
  
  void initLCD() {
    pinMode(lcdRS, OUTPUT);
    pinMode(lcdEN, OUTPUT);
    pinMode(lcdD4, OUTPUT);
    pinMode(lcdD5, OUTPUT);
    pinMode(lcdD6, OUTPUT);
    pinMode(lcdD7, OUTPUT);
    
    delay(50);
    
    writeNibble(0x03);
    delay(5);
    writeNibble(0x03);
    delayMicroseconds(150);
    writeNibble(0x03);
    writeNibble(0x02);
    
    sendCommand(0x28);
    sendCommand(0x0C);
    sendCommand(0x06);
    sendCommand(0x01);
    delay(2);
  }
  
  void writeNibble(byte nibble) {
    digitalWrite(lcdD4, (nibble >> 0) & 1);
    digitalWrite(lcdD5, (nibble >> 1) & 1);
    digitalWrite(lcdD6, (nibble >> 2) & 1);
    digitalWrite(lcdD7, (nibble >> 3) & 1);
    
    digitalWrite(lcdEN, HIGH);
    delayMicroseconds(1);
    digitalWrite(lcdEN, LOW);
    delayMicroseconds(50);
  }
  
  void sendCommand(byte cmd) {
    digitalWrite(lcdRS, LOW);
    writeNibble(cmd >> 4);
    writeNibble(cmd & 0x0F);
    delayMicroseconds(2000);
  }
  
  void sendData(byte data) {
    digitalWrite(lcdRS, HIGH);
    writeNibble(data >> 4);
    writeNibble(data & 0x0F);
    delayMicroseconds(50);
  }
  
  void setCursor(byte col, byte row) {
    byte address = (row == 0) ? 0x80 : 0xC0;
    sendCommand(address + col);
  }
  
  void print(const char* str) {
    while (*str) {
      sendData(*str++);
    }
  }
  
  void printNumber(uint32_t num, byte width) {
    char buffer[12];
    ultoa(num, buffer, 10);
    
    byte len = strlen(buffer);
    for (byte i = 0; i < width - len; i++) {
      sendData(' ');
    }
    print(buffer);
  }
  
  void setupWDT() {
    uint8_t wdtTimeout;
    
    switch(currentMode) {
      case 0: wdtTimeout = (1 << WDP2) | (1 << WDP1); break;
      case 1: wdtTimeout = (1 << WDP2) | (1 << WDP0); break;
      case 2: wdtTimeout = (1 << WDP2); break;
      case 3: wdtTimeout = (1 << WDP1) | (1 << WDP0); break;
      default: wdtTimeout = (1 << WDP2) | (1 << WDP1); break;
    }
    
    cli();
    
    MCUSR &= ~(1 << WDRF);
    
    WDTCSR = (1 << WDCE) | (1 << WDE);
    
    WDTCSR = (1 << WDIE) | wdtTimeout;
    
    sei();
  }
  
  void updateWDTMode() {
    uint8_t wdtTimeout;
    
    switch(currentMode) {
      case 0: wdtTimeout = (1 << WDP2) | (1 << WDP1); break;
      case 1: wdtTimeout = (1 << WDP2) | (1 << WDP0); break;
      case 2: wdtTimeout = (1 << WDP2); break;
      case 3: wdtTimeout = (1 << WDP1) | (1 << WDP0); break;
      default: wdtTimeout = (1 << WDP2) | (1 << WDP1); break;
    }
    
    cli();
    
    MCUSR &= ~(1 << WDRF);
    
    WDTCSR = (1 << WDCE) | (1 << WDE);
    
    WDTCSR = (1 << WDIE) | wdtTimeout;
    
    sei();
  }
  
  void handleWDTInterrupt() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    wdtCounter++;
  }
  
  void update() {
    bool buttonState = digitalRead(buttonPin);
    
    if (buttonState == LOW && lastButtonState == HIGH) {
      delay(50);
      currentMode = (currentMode + 1) % 4;
      updateWDTMode();
      wdtCounter = 0;
    }
    
    lastButtonState = buttonState;
    
    if (millis() - lastLCDUpdate >= 100) {
      updateDisplay();
      lastLCDUpdate = millis();
    }
  }
  
  void updateDisplay() {
    uint32_t currentCounter;
    uint8_t wdtcsrValue;
    
    cli();
    currentCounter = wdtCounter;
    wdtcsrValue = WDTCSR;
    sei();
    
    setCursor(0, 0);
    print("WDT MD: ");
    print(modeNames[currentMode]);

    setCursor(16, 0);
    print("WDTCSR (HEX):");
    printHex(wdtcsrValue);
    
    setCursor(0, 1);
    print("ISR:");
    printNumber(currentCounter, 5);
 
  }
  
  void printHex(byte value) {
    const char hexChars[] = "0123456789ABCDEF";
    sendData(hexChars[(value >> 4) & 0x0F]);
    sendData(hexChars[value & 0x0F]);
  }
  
  static WDTDemo* instance;
};

WDTDemo* WDTDemo::instance = nullptr;
WDTDemo demo;

ISR(WDT_vect) {
  WDTDemo::instance->handleWDTInterrupt();
}

void setup() {
  WDTDemo::instance = &demo;
  demo.begin();
}

void loop() {
  demo.update();
  delay(10);
}