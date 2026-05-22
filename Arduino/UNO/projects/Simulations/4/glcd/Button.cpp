#include "Button.h"

Button::Button(uint8_t p) : pin(p), lastState(LOW), lastDebounce(0) {
  pinMode(pin, INPUT);
}

bool Button::isPressed() {
  bool current = digitalRead(pin);
  bool pressed = false;
  
  if (current != lastState) {
    lastDebounce = millis();
  }
  
  if ((millis() - lastDebounce) > DEBOUNCE_DELAY) {
    if (current == HIGH) {
       pressed = true;
    }
  }
  
  lastState = current;
  return pressed;
}