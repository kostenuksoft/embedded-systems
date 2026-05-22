#ifndef __KS0108__GLCD_H
#define __KS0108__GLCD_H

#include <Arduino.h>
#include "Config.h"

class GLCD {
private:
  uint8_t dataPins[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  uint8_t buffer[128][8];
  
  void writeCommand(uint8_t cmd, uint8_t chip);
  void writeData(uint8_t data, uint8_t chip);
  void writeByte(uint8_t value);
  void selectChip(uint8_t chip);

public:
  GLCD();
  void begin();
  void clear();
  void setPixel(int16_t x, int16_t y, uint8_t color);
  void drawLine(int x0, int y0, int x1, int y1);
  void drawCircle(int xc, int yc, int r);
};

#endif