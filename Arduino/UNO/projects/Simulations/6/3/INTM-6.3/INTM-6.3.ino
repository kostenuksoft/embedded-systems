class ADCKeypadHandler {
private:
  const byte displayPins[4] = {8, 9, 10, 11};
  volatile byte lastKey;
  volatile bool newKeyDetected;
  
  byte currentColumn;

public:
  ADCKeypadHandler() : lastKey(0xFF), newKeyDetected(false), currentColumn(0) {}
  
  void begin() {
    for (byte i = 0; i < 4; i++) {
      pinMode(displayPins[i], OUTPUT);
    }
    
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    displayDigit(0);
    startConversion();
  }
  
  void startConversion() {
    ADMUX = (ADMUX & 0xF0) | currentColumn;
    ADCSRA |= (1 << ADSC);
  }
  
  void handleADCComplete() {
    uint16_t adcValue = ADC;
    byte row = determineRow(adcValue);
    
    if (row != 0xFF) {
      byte key = row * 4 + currentColumn;
      if (key != lastKey) {
        lastKey = key;
        newKeyDetected = true;
      }
    }
    
    currentColumn = (currentColumn + 1) % 4;
  }
  
  byte determineRow(uint16_t adcValue) {
    if (adcValue < 50) return 0xFF;
    
    if (adcValue >= 50 && adcValue < 300) return 3;
    if (adcValue >= 300 && adcValue < 550) return 2;
    if (adcValue >= 550 && adcValue < 800) return 1;
    if (adcValue >= 800) return 0;
    
    return 0xFF;
  }
  
  void update() {
    if (newKeyDetected) {
      newKeyDetected = false;
      displayDigit(lastKey);
    }
    startConversion();
  }
  
  void displayDigit(byte digit) {
    if (digit > 15) digit = 0;
    
    for (byte i = 0; i < 4; i++) {
      digitalWrite(displayPins[i], (digit >> i) & 1);
    }
  }
  
  static ADCKeypadHandler* instance;
};

ADCKeypadHandler* ADCKeypadHandler::instance = nullptr;
ADCKeypadHandler keypad;

ISR(ADC_vect) {
  ADCKeypadHandler::instance->handleADCComplete();
}

void setup() {
  ADCKeypadHandler::instance = &keypad;
  keypad.begin();
}

void loop() {
  keypad.update();
  delay(10);
}