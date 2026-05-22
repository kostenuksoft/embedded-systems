#include <LiquidCrystal.h>

class LCDAnimator {
private:
  LiquidCrystal lcd;
  static const int COLS = 40;
  static const int ROWS = 2;
  
  String topText;
  String bottomText;
  int topPos;
  int bottomPos;
  
public:
  LCDAnimator() : lcd(12, 11, 5, 4, 3, 2),
                  topText("Vladyslav"),
                  bottomText("Repetovskyi") {}
  
  void begin() {
    lcd.begin(COLS, ROWS);
    lcd.clear();
    
    topPos = (COLS - topText.length()) / 2;     
    bottomPos = (COLS - bottomText.length()) / 2; 
  }
  
  void update() {
    lcd.clear();
    
    drawText(topText, topPos, 0);
    
    drawText(bottomText, bottomPos, 1);
    
    topPos--;
    if (topPos < 0) {
      topPos = COLS - 1;
    }
    
    bottomPos++;
    if (bottomPos >= COLS) {
      bottomPos = 0;
    }
    
    delay(250);
  }
  
private:
  void drawText(const String& text, int startPos, int row) {
    for (int i = 0; i < text.length(); i++) {
      int charPos = (startPos + i) % COLS;
      lcd.setCursor(charPos, row);
      lcd.print(text[i]);
    }
  }
};

LCDAnimator animator;

void setup() {
  animator.begin();
}

void loop() {
  animator.update();
}