#include "GLCD.h"

GLCD::GLCD() {
  memset(buffer, 0, sizeof(buffer));
}

void GLCD::writeCommand(uint8_t cmd, uint8_t chip) {
  digitalWrite(GLCD_RS_PIN, LOW);
  digitalWrite(GLCD_RW_PIN, LOW);
  selectChip(chip);
  writeByte(cmd);
  digitalWrite(GLCD_EN_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(GLCD_EN_PIN, LOW);
  delayMicroseconds(1);
}

void GLCD::writeData(uint8_t data, uint8_t chip) {
  digitalWrite(GLCD_RS_PIN, HIGH);
  digitalWrite(GLCD_RW_PIN, LOW);
  selectChip(chip);
  writeByte(data);
  digitalWrite(GLCD_EN_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(GLCD_EN_PIN, LOW);
  delayMicroseconds(1);
}

void GLCD::writeByte(uint8_t value) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(dataPins[i], (value >> i) & 0x01);
  }
}

void GLCD::selectChip(uint8_t chip) {
  if (chip == 0) {
    digitalWrite(GLCD_CS1_PIN, HIGH);
    digitalWrite(GLCD_CS2_PIN, LOW);
  } else {
    digitalWrite(GLCD_CS1_PIN, LOW);
    digitalWrite(GLCD_CS2_PIN, HIGH);
  }
}

void GLCD::begin() {
  pinMode(GLCD_RS_PIN, OUTPUT);
  pinMode(GLCD_RW_PIN, OUTPUT);
  pinMode(GLCD_EN_PIN, OUTPUT);
  pinMode(GLCD_CS1_PIN, OUTPUT);
  pinMode(GLCD_CS2_PIN, OUTPUT);
  pinMode(GLCD_RST_PIN, OUTPUT);
  
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(dataPins[i], OUTPUT);
  }
  
  digitalWrite(GLCD_RST_PIN, LOW);
  delay(10);
  digitalWrite(GLCD_RST_PIN, HIGH);
  delay(10);
  
  for (uint8_t chip = 0; chip < 2; chip++) {
    writeCommand(0x3F, chip);
    writeCommand(0xC0, chip);
    delay(1);
  }
  
  clear();
}

void GLCD::clear() {
  for (uint8_t page = 0; page < 8; page++) {
    for (uint8_t chip = 0; chip < 2; chip++) {
      writeCommand(0xB8 | page, chip);
      writeCommand(0x40, chip);
      for (uint8_t col = 0; col < 64; col++) {
        writeData(0x00, chip);
      }
    }
  }
  memset(buffer, 0, sizeof(buffer));
}

void GLCD::setPixel(int16_t x, int16_t y, uint8_t color) {
  if (x < 0 || x >= GLCD_WIDTH || y < 0 || y >= GLCD_HEIGHT) return;
  
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  
  if (color) {
    buffer[x][page] |= (1 << bit);
  } else {
    buffer[x][page] &= ~(1 << bit);
  }
  
  uint8_t chip = (x < 64) ? 0 : 1;
  uint8_t localX = (x < 64) ? x : x - 64;
  
  writeCommand(0xB8 | page, chip);
  writeCommand(0x40 | localX, chip);
  writeData(buffer[x][page], chip);
}

void GLCD::drawLine(int x0, int y0, int x1, int y1) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  
  while (true) {
    setPixel(x0, y0, 1);
    if (x0 == x1 && y0 == y1) break;
    
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void GLCD::drawCircle(int xc, int yc, int r) {
  int x = 0, y = r;
  int d = 3 - 2 * r;
  
  while (x <= y) {
    setPixel(xc + x, yc + y, 1);
    setPixel(xc - x, yc + y, 1);
    setPixel(xc + x, yc - y, 1);
    setPixel(xc - x, yc - y, 1);
    setPixel(xc + y, yc + x, 1);
    setPixel(xc - y, yc + x, 1);
    setPixel(xc + y, yc - x, 1);
    setPixel(xc - y, yc - x, 1);
    
    x++;
    d = (d < 0) ? d + 4 * x + 6 : d + 4 * (x - y--) + 10;
  }
}