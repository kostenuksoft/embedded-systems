#include <LedControl.h>

const int DIN_PIN = 12;   
const int CLK_PIN = 11; 
const int CS_PIN = 10;   

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

byte char_R[8] = {
  B11111000,
  B10001000,
  B10001000,
  B11111000,
  B10000000,
  B10000000,
  B10000000,
  B00000000
};

byte char_e[8] = {
  B00000000,
  B01110000,
  B10001000,
  B11111000,
  B10000000,
  B01111000,
  B00000000,
  B00000000
};

byte char_p[8] = {
  B00000000,
  B11111000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B00000000,
  B00000000
};

byte char_e2[8] = {
  B00000000,
  B01110000,
  B10001000,
  B11111000,
  B10000000,
  B01111000,
  B00000000,
  B00000000
};

byte char_t[8] = {
  B00000000,
  B11111000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00000000,
  B00000000
};

byte char_o[8] = {
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  B00000000,
  B00000000
};

byte char_v[8] = {
  B00000000,
  B11110000,
  B10001000,
  B11110000,
  B10001000,
  B11110000,
  B00000000,
  B00000000
};

byte char_s[8] = {
  B00000000,
  B01110000,
  B10000000,
  B10000000,
  B10000000,
  B01110000,
  B00000000,
  B00000000
};

byte char_soft[8] = {
  B00000000,
  B10000000,
  B10000000,
  B11110000,
  B10001000,
  B11110000,
  B00000000,
  B00000000
};

byte char_k[8] = {
  B00000000,
  B10001000,
  B10010000,
  B11100000,
  B10010000,
  B10001000,
  B00000000,
  B00000000
};

byte char_i[8] = {
  B00000000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B00000000,
  B00000000
};

byte char_y[8] = {
  B01010000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B00000000,
  B00000000
};

byte char_space[8] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};


byte* surname[] = {
  char_R,     
  char_e,     
  char_p,     
  char_e2,    
  char_t,     
  char_o,     
  char_v,    
  char_s,     
  char_soft,  
  char_k,     
  char_i,     
  char_y,     
  char_space, 
  char_space  
};

const int numChars = sizeof(surname) / sizeof(surname[0]);

int scrollPosition = 0;
unsigned long previousMillis = 0;
const long scrollDelay = 150; 

void setup() {
  Serial.begin(9600);
  
  lc.shutdown(0, false); 
  lc.setIntensity(0, 8);   
  lc.clearDisplay(0);
  
  Serial.println("Scrolling Cyrillic text: Репетовський");
}

void loop() {
  unsigned long currentMillis = millis();
  

  if (currentMillis - previousMillis >= scrollDelay) {
    previousMillis = currentMillis;
    lc.clearDisplay(0);
    displayScrollingText();
    scrollPosition++;
    if (scrollPosition >= numChars * 8 + 8) {
      scrollPosition = 0;
    }
  }
}

void displayScrollingText() {
  int charIndex = scrollPosition / 8;
  int columnOffset = scrollPosition % 8;
  
  for (int displayCol = 0; displayCol < 8; displayCol++) {
    int sourceCharIndex = charIndex + (displayCol + columnOffset) / 8;
    int sourceColumn = (displayCol + columnOffset) % 8;
    
    if (sourceCharIndex < numChars) {
      byte columnData = 0;
      for (int row = 0; row < 8; row++) {
        if (surname[sourceCharIndex][row] & (1 << (7 - sourceColumn))) {
          columnData |= (1 << row);
        }
      }
      lc.setColumn(0, 7 - displayCol, columnData);
    }
  }
}

void setPixel(int row, int col, bool state) {
  lc.setLed(0, row, col, state);
}

void displayCustomPattern(byte pattern[8]) {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, pattern[row]);
  }
}

void createCustomChar(byte charPattern[8]) {
  
  for (int i = 0; i < 8; i++) {
    charPattern[i] = B00000000;
  }
  
  charPattern[0] = B11111110;
  charPattern[1] = B10000010;
  charPattern[2] = B10000010;
  charPattern[3] = B11111110;
  charPattern[4] = B10001000;
  charPattern[5] = B10000100;
  charPattern[6] = B10000010;
  charPattern[7] = B00000000;
}

void testAllCharacters() {
  for (int i = 0; i < numChars; i++) {
    displayCustomPattern(surname[i]);
    delay(1000);
    lc.clearDisplay(0);
    delay(200);
  }
}