// Matrix Keyboard with External Interrupt Handler
// Task 6.2 - OOP Implementation

class KeypadInterruptHandler {
private:
  const byte interruptPin = 2;      // INT0 (PD2)
  const byte interruptControlPin = 13; // Interrupt enable/disable control input
  const byte dataPin0 = 8;          // First data pin
  const byte dataPin1 = 9;
  const byte dataPin2 = 10;
  const byte dataPin3 = 11;
  
  volatile byte keyNumber;
  volatile bool keyPressed;
  volatile unsigned long lastInterruptTime;
  const unsigned long debounceDelay = 50;
  bool lastPin13State;
  
  // 7-segment BCD display pins
  const byte bcdPins[4] = {A0, A1, A2, A3}; // D0-D3 BCD inputs

public:
  KeypadInterruptHandler() : keyNumber(0xFF), keyPressed(false), lastInterruptTime(0), lastPin13State(false) {}
  
  void begin() {
    // Setup interrupt control pin as INPUT
    pinMode(interruptControlPin, INPUT);
    
    // Setup data input pins
    pinMode(dataPin0, INPUT);
    pinMode(dataPin1, INPUT);
    pinMode(dataPin2, INPUT);
    pinMode(dataPin3, INPUT);
    
    // Setup BCD display pins
    for (byte i = 0; i < 4; i++) {
      pinMode(bcdPins[i], OUTPUT);
    }
    
    // Setup interrupt pin
    pinMode(interruptPin, INPUT_PULLUP);
    
    // Attach interrupt permanently (FALLING edge when key is pressed)
    attachInterrupt(digitalPinToInterrupt(interruptPin), 
                   KeypadInterruptHandler::staticISR, FALLING);
    
    // Set initial interrupt state based on pin 13
    lastPin13State = digitalRead(interruptControlPin);
    if (lastPin13State) {
      sei(); // Enable global interrupts
    } else {
      cli(); // Disable global interrupts
    }
    
    displayDigit(0); // Show 0 initially
  }
  
  static void staticISR() {
    instance->handleInterrupt();
  }
  
  void handleInterrupt() {
    unsigned long currentTime = millis();
    
    // Debounce check
    if (currentTime - lastInterruptTime > debounceDelay) {
      // Read key number from data pins (4-bit value)
      keyNumber = (digitalRead(dataPin3) << 3) |
                  (digitalRead(dataPin2) << 2) |
                  (digitalRead(dataPin1) << 1) |
                  digitalRead(dataPin0);
      
      keyPressed = true;
      lastInterruptTime = currentTime;
    }
  }
  
  void update() {
    // Read pin 13 state
    bool currentPin13State = digitalRead(interruptControlPin);
    
    // Only change interrupt state when pin 13 changes
    if (currentPin13State != lastPin13State) {
      lastPin13State = currentPin13State;
      
      if (currentPin13State) {
        sei(); // Enable global interrupts (set I-flag in SREG)
      } else {
        cli(); // Disable global interrupts (clear I-flag in SREG)
      }
    }
    
    // Process key press if detected
    if (keyPressed) {
      keyPressed = false;
      displayDigit(keyNumber);
    }
  }
  
  void displayDigit(byte digit) {
    // Clamp to valid 4-bit range (0-15)
    if (digit > 15) digit = 0;
    
    // Output full 4-bit binary value to BCD pins
    // This will display:
    // 0-9: normal digits
    // 10-15: A, b, C, d, E, F (on 7-segment decoders that support hex)
    for (byte i = 0; i < 4; i++) {
      digitalWrite(bcdPins[i], (digit >> i) & 1);
    }
  }
  
  byte getKeyNumber() const {
    return keyNumber;
  }
  
  static KeypadInterruptHandler* instance;
};

// Static instance pointer
KeypadInterruptHandler* KeypadInterruptHandler::instance = nullptr;

// Global object
KeypadInterruptHandler keypad;

void setup() {
  KeypadInterruptHandler::instance = &keypad;
  keypad.begin();
}

void loop() {
  keypad.update();
  delay(10);
}