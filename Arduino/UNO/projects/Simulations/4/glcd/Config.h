#ifndef __CONFIG_H
#define __CONFIG_H

#include <Arduino.h>

#define GLCD_RS_PIN 8
#define GLCD_RW_PIN 9
#define GLCD_EN_PIN 10
#define GLCD_CS1_PIN 11
#define GLCD_CS2_PIN 12
#define GLCD_RST_PIN 13

#define GLCD_D0_PIN 0
#define GLCD_D1_PIN 1
#define GLCD_D2_PIN 2
#define GLCD_D3_PIN 3
#define GLCD_D4_PIN 4
#define GLCD_D5_PIN 5
#define GLCD_D6_PIN 6
#define GLCD_D7_PIN 7

#define GLCD_WIDTH 128
#define GLCD_HEIGHT 64

#define BUTTON_PIN A0
#define DEBOUNCE_DELAY 50

#define CENTER_X 96
#define CENTER_Y 32

#endif