#include <Arduino.h>

#define ADC_INPUT A0

#define DAC_D0 2
#define DAC_D1 3
#define DAC_D2 4
#define DAC_D3 5
#define DAC_D4 6
#define DAC_D5 7
#define DAC_D6 8
#define DAC_D7 9
#define DAC_D8 10
#define DAC_D9 11

#define DAC_LE 13

const uint8_t dacPins[10] = {
  DAC_D0, DAC_D1, DAC_D2, DAC_D3, DAC_D4,
  DAC_D5, DAC_D6, DAC_D7, DAC_D8, DAC_D9
};

class DAC10Bit {
private:
  const uint8_t* pins;
  uint8_t lePin;
  
public:
  DAC10Bit(const uint8_t* dataPins, uint8_t latchPin) 
    : pins(dataPins), lePin(latchPin) {}
  
  void begin() {
    for (uint8_t i = 0; i < 10; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], LOW);
    }
    pinMode(lePin, OUTPUT);
    digitalWrite(lePin, LOW);
  }
  
  void write(uint16_t value) {
    value = constrain(value, 0, 1023);
    
    digitalWrite(lePin, LOW);
    
    for (uint8_t i = 0; i < 10; i++) {
      digitalWrite(pins[i], (value >> i) & 0x01);
    }
    
    digitalWrite(lePin, HIGH);
    delayMicroseconds(1);
    digitalWrite(lePin, LOW);
  }
};

DAC10Bit dac(dacPins, DAC_LE);

void setup() {
  dac.begin();
  pinMode(ADC_INPUT, INPUT);
  analogReference(DEFAULT);
  for (int i = 0; i < 10; i++) {
    analogRead(ADC_INPUT);
    delay(10);
  }
}

void loop() 
{
  uint16_t adcValue = analogRead(ADC_INPUT);
  dac.write(adcValue);
}
