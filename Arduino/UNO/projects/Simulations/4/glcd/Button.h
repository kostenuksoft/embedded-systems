#ifndef __BUTTON_H
#define __BUTTON_H

#include <Arduino.h>
#include "Config.h"

class Button {
private:
  uint8_t pin;
  bool lastState;
  unsigned long lastDebounce;
  
public:
  Button(uint8_t p);
  bool isPressed();
};

#endif