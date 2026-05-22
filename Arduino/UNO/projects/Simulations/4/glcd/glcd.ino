#include "Config.h"
#include "GLCD.h"
#include "AnimationManager.h"
#include "Button.h"

GLCD glcd;
AnimationManager animManager;
Button button(BUTTON_PIN);

void setup() {
  glcd.begin();
  glcd.clear();
  animManager.switchAnimation();
}

void loop() {
  if (button.isPressed()) {
    animManager.switchAnimation();
  }
  animManager.update();
  delay(16);
}