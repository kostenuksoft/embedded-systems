#include <Arduino.h>

#define ADC_INPUT A0

#define PWM_OUTPUT 9

void setup() {
  pinMode(ADC_INPUT, INPUT);
  analogReference(DEFAULT);  
  
  pinMode(PWM_OUTPUT, OUTPUT);
  
  TCCR1A = _BV(COM1A1) | _BV(WGM11);       
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); 
  ICR1 = 1023;  
  OCR1A = 0;   
  
  for (int i = 0; i < 10; i++) {
    analogRead(ADC_INPUT);
  }
}

void loop() {
  uint16_t adcValue = analogRead(ADC_INPUT);
  
  OCR1A = adcValue;
  
}
