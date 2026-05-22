#include <Arduino.h>

#define ADC_INPUT A0

// LCD pins
#define LCD_D0 0
#define LCD_D1 1
#define LCD_D2 2
#define LCD_D3 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_RS 8
#define LCD_RW 9
#define LCD_E 10
#define LCD_CS1 11
#define LCD_CS2 12
#define LCD_RST 13

#define SAMPLES 128

class GLCD128x64 {
private:
  uint8_t dataPins[8] = {LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7};
  uint8_t buffer[128][8];
  
  void writeByte(uint8_t value) {
    for (uint8_t i = 0; i < 8; i++) {
      digitalWrite(dataPins[i], (value >> i) & 0x01);
    }
  }
  
  void selectChip(uint8_t chip) {
    if (chip == 0) {
      digitalWrite(LCD_CS1, HIGH);
      digitalWrite(LCD_CS2, LOW);
    } else {
      digitalWrite(LCD_CS1, LOW);
      digitalWrite(LCD_CS2, HIGH);
    }
  }
  
  void pulse() {
    digitalWrite(LCD_E, HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD_E, LOW);
    delayMicroseconds(1);
  }
  
  void writeCommand(uint8_t cmd, uint8_t chip) {
    selectChip(chip);
    digitalWrite(LCD_RS, LOW);
    digitalWrite(LCD_RW, LOW);
    writeByte(cmd);
    pulse();
  }
  
  void writeData(uint8_t data, uint8_t chip) {
    selectChip(chip);
    digitalWrite(LCD_RS, HIGH);
    digitalWrite(LCD_RW, LOW);
    writeByte(data);
    pulse();
  }
  
public:
  void begin() {
    pinMode(LCD_RS, OUTPUT);
    pinMode(LCD_RW, OUTPUT);
    pinMode(LCD_E, OUTPUT);
    pinMode(LCD_CS1, OUTPUT);
    pinMode(LCD_CS2, OUTPUT);
    pinMode(LCD_RST, OUTPUT);
    
    for (uint8_t i = 0; i < 8; i++) {
      pinMode(dataPins[i], OUTPUT);
    }
    
    digitalWrite(LCD_RST, LOW);
    delay(10);
    digitalWrite(LCD_RST, HIGH);
    delay(50);
    
    for (uint8_t chip = 0; chip < 2; chip++) {
      writeCommand(0x3F, chip);  // Display ON
      writeCommand(0xC0, chip);  // Start line 0
    }
    
    clearBuffer();
    display();
  }
  
  void clearBuffer() {
    memset(buffer, 0, sizeof(buffer));
  }
  
  void setPixel(uint8_t x, uint8_t y) {
    if (x >= 128 || y >= 64) return;
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    buffer[x][page] |= (1 << bit);
  }
  
  void display() {
    for (uint8_t page = 0; page < 8; page++) {
      
      selectChip(1);
      writeCommand(0xB8 | page, 1);  // Set page
      writeCommand(0x40, 1);          // Set column 0
      for (uint8_t x = 0; x < 64; x++) {
        digitalWrite(LCD_RS, HIGH);
        digitalWrite(LCD_RW, LOW);
        writeByte(buffer[x][page]);
        pulse();
      }
      
      selectChip(0);
      writeCommand(0xB8 | page, 0);  // Set page
      writeCommand(0x40, 0);          // Set column 0
      for (uint8_t x = 64; x < 128; x++) {
        digitalWrite(LCD_RS, HIGH);
        digitalWrite(LCD_RW, LOW);
        writeByte(buffer[x][page]);
        pulse();
      }
    }
  }
};

GLCD128x64 lcd;
uint16_t samples[SAMPLES];

void setup() {
  pinMode(ADC_INPUT, INPUT);
  analogReference(DEFAULT);
  
  for (int i = 0; i < 10; i++) {
    analogRead(ADC_INPUT);
    delay(5);
  }
  
  lcd.begin();
  delay(100);
}

void loop() {
  // IMPORTANT: Capture all 128 samples in ONE continuous burst
  // This ensures the waveform is continuous without phase jumps
  for (uint16_t i = 0; i < SAMPLES; i++) {
    samples[i] = analogRead(ADC_INPUT);
    delayMicroseconds(100);  // Fixed sampling rate
  }
  
  lcd.clearBuffer();
  

  // Connect adjacent points with lines for smoothness
  for (uint16_t x = 0; x < SAMPLES - 1; x++) {
    uint8_t y1 = map(samples[x], 0, 1023, 63, 0);
    uint8_t y2 = map(samples[x + 1], 0, 1023, 63, 0);
    
    y1 = constrain(y1, 0, 63);
    y2 = constrain(y2, 0, 63);
    
    // Fill the gap between points
    int yStart = min(y1, y2);
    int yEnd = max(y1, y2);
    
    for (int y = yStart; y <= yEnd; y++) {
      lcd.setPixel(x, y);
    }
  }
  
  lcd.display();
  
  delay(100);
}
