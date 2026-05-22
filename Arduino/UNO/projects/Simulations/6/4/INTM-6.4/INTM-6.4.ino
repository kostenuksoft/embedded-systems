class TMCMP {
private:
  const byte ledPin = 13;
  const byte lcdRS = 7;
  const byte lcdEN = 9;
  const byte lcdD4 = 10;
  const byte lcdD5 = 11;
  const byte lcdD6 = 12;
  const byte lcdD8 = 8;
  const byte buttonPin = 2;
  
  volatile uint32_t interruptCounter;
  volatile bool ledState;
  
  byte currentMode;
  bool lastButtonState;
  unsigned long lastLCDUpdate;
  
  const uint16_t compareValues[4] = {
    15624,
    7812,
    3906,
    1953
  };
  
  const char* modeNames[4] = {
    "1Hz ",
    "2Hz ",
    "4Hz ",
    "8Hz "
  };

public:
  TMCMP() : 
    interruptCounter(0), 
    ledState(false), 
    currentMode(0),
    lastButtonState(HIGH),
    lastLCDUpdate(0) {}
  
  void begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    
    initLCD();
    setupTimer1();
    updateDisplay();
  }
  
  void initLCD() {
    pinMode(lcdRS, OUTPUT);
    pinMode(lcdEN, OUTPUT);
    pinMode(lcdD4, OUTPUT);
    pinMode(lcdD5, OUTPUT);
    pinMode(lcdD6, OUTPUT);
    pinMode(lcdD8, OUTPUT);
    
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
    digitalWrite(lcdD8, (nibble >> 3) & 1);
    
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
  
  void setupTimer1() {
    cli();
    
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    
    OCR1A = compareValues[currentMode];
    
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    
    sei();
  }
  
  void handleCompareMatch() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    interruptCounter++;
  }
  
  void update() {
    bool buttonState = digitalRead(buttonPin);
    
    if (buttonState == LOW && lastButtonState == HIGH) {
      delay(50);
      currentMode = (currentMode + 1) % 4;
      updateTimerFrequency();
      interruptCounter = 0;
    }
    
    lastButtonState = buttonState;
    
    if (millis() - lastLCDUpdate >= 100) {
      updateDisplay();
      lastLCDUpdate = millis();
    }
  }
  
  void updateTimerFrequency() {
    cli();
    OCR1A = compareValues[currentMode];
    TCNT1 = 0;
    sei();
  }
  
  void updateDisplay() {
    uint16_t currentTCNT1;
    uint16_t currentOCR1A;
    uint32_t currentCounter;
    
    cli();
    uint8_t tcnt1_low = TCNT1L;
    uint8_t tcnt1_high = TCNT1H;
    currentTCNT1 = (tcnt1_high << 8) | tcnt1_low;
    currentOCR1A = OCR1A;
    currentCounter = interruptCounter;
    sei();
    
    setCursor(0, 0);
    print("ISR:");
    printNumber(currentCounter, 5);

    setCursor(24, 0);
    print("MD:");
    print(modeNames[currentMode]);
    
    setCursor(0, 1);
    print("TCNT:");
    printNumber(currentTCNT1, 5);

    setCursor(24, 1);
    print("OCR:");
    printNumber(currentOCR1A, 5);   
  }
  
  static TMCMP* instance;
};

TMCMP* TMCMP::instance = nullptr;
TMCMP demo;

ISR(TIMER1_COMPA_vect) {
  TMCMP::instance->handleCompareMatch();
}

void setup() {
  TMCMP::instance = &demo;
  demo.begin();
}

void loop() {
  demo.update();
  delay(10);
}