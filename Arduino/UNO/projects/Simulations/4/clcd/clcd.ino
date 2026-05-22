#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(12, 11, 0, 1, 2, 3, 4, 5, 6, 7);

const byte ROWS = 4;
const byte COLS = 6;


char keys[ROWS][COLS] = {
  {'C', '7', '8', '9', '*', '/'},
  {'N', '4', '5', '6', '-', 'R'},
  {'%', '1', '2', '3', '+', 'S'},
  {'Q', '0', '.', '=', '+', 'A'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {8, 9, 10, 13, A4, A5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

class Calculator {
private:
  double currentValue;
  double storedValue;
  double memory;
  char currentOperation;
  bool newNumber;
  bool hasDecimal;
  String displayBuffer;
  int mrcPressCount;
  static const byte MAX_DIGITS = 16;

  void resetInput() {
    displayBuffer = "";
    hasDecimal = false;
    newNumber = true;
  }

  double performOperation(double val1, double val2, char op) {
    switch(op) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return (val2 != 0) ? val1 / val2 : 0;
      default: return val2;
    }
  }

  void updateDisplay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (memory != 0) {
      lcd.print("M");
    }
    
    if (currentOperation != '\0') {
      lcd.setCursor(2, 0);
      if (abs(storedValue) < 1000000) {
        lcd.print(storedValue, 2);
      } else {
        lcd.print(storedValue, 0);
      }
      lcd.print(' ');
      lcd.print(currentOperation);
    }
    
    lcd.setCursor(0, 1);
    if (displayBuffer.length() > 0) {
      lcd.print(displayBuffer);
    } else {
      if (abs(currentValue) < 1000000) {
        lcd.print(currentValue, 6);
        String str = String(currentValue, 6);
        while (str.endsWith("0") && str.indexOf('.') != -1) {
          str.remove(str.length() - 1);
        }
        if (str.endsWith(".")) {
          str.remove(str.length() - 1);
        }
        lcd.setCursor(0, 1);
        lcd.print(str);
      } else {
        lcd.print(currentValue, 2);
      }
    }
  }

  void showError(const char* msg) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERROR:");
    lcd.setCursor(0, 1);
    lcd.print(msg);
    delay(2000);
    clear();
  }

public:
  Calculator() : currentValue(0), storedValue(0), memory(0), 
                 currentOperation('\0'), newNumber(true), 
                 hasDecimal(false), mrcPressCount(0) {
    displayBuffer = "";
  }

  void init() {
    delay(100);
    lcd.begin(16, 2);
    delay(50);
    lcd.clear();
    delay(50);
    
    lcd.setCursor(0, 0);
    lcd.print("Repetovskyi Calc");
    lcd.setCursor(0, 1);
    lcd.print("Ready...");
    delay(1500);
    
  }

  void handleDigit(char digit) {
    if (newNumber) {
      displayBuffer = "";
      newNumber = false;
    }
    
    if (displayBuffer.length() < MAX_DIGITS) {
      displayBuffer += digit;
      currentValue = displayBuffer.toDouble();
      updateDisplay();
    }
    mrcPressCount = 0;
  }

  void handleDecimal() {
    if (newNumber) {
      displayBuffer = "0";
      newNumber = false;
    }
    
    if (!hasDecimal && displayBuffer.length() < MAX_DIGITS - 1) {
      displayBuffer += '.';
      hasDecimal = true;
      updateDisplay();
    }
    mrcPressCount = 0;
  }

  void handleOperation(char op) {
    if (currentOperation != '\0') {
      calculate();
    } else {
      storedValue = currentValue;
    }
    
    currentOperation = op;
    resetInput();
    updateDisplay();
    mrcPressCount = 0;
  }

  void calculate() {
    if (currentOperation != '\0') {
      if (currentOperation == '/' && currentValue == 0) {
        showError("Div by zero");
        clear();
        return;
      }
      
      currentValue = performOperation(storedValue, currentValue, currentOperation);
      storedValue = currentValue;
      currentOperation = '\0';
      resetInput();
      updateDisplay();
    }
    mrcPressCount = 0;
  }

  void clear() {
    currentValue = 0;
    storedValue = 0;
    currentOperation = '\0';
    displayBuffer = "";
    resetInput();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("0");
    mrcPressCount = 0;
  }

  void percentage() {
    if (currentOperation == '+' || currentOperation == '-') {
      currentValue = (storedValue * currentValue) / 100.0;
    } else if (currentOperation == '*' || currentOperation == '/') {
      currentValue = currentValue / 100.0;
    } else {
      currentValue = currentValue / 100.0;
    }
    displayBuffer = "";
    updateDisplay();
    mrcPressCount = 0;
  }

  void squareRoot() {
    if (currentValue >= 0) {
      currentValue = sqrt(currentValue);
      displayBuffer = "";
      newNumber = true;
      updateDisplay();
    } else {
      showError("Invalid Input");
    }
    mrcPressCount = 0;
  }

  void changeSign() {
    currentValue = -currentValue;
    if (displayBuffer.length() > 0) {
      if (displayBuffer.charAt(0) == '-') {
        displayBuffer.remove(0, 1);
      } else {
        displayBuffer = "-" + displayBuffer;
      }
    }
    updateDisplay();
    mrcPressCount = 0;
  }

  void memoryAdd() {
    memory += currentValue;
    updateDisplay();
    mrcPressCount = 0;
  }

  void memorySubtract() {
    memory -= currentValue;
    updateDisplay();
    mrcPressCount = 0;
  }

  void memoryRecallClear() {
    mrcPressCount++;
    
    if (mrcPressCount == 1) {
      currentValue = memory;
      displayBuffer = "";
      newNumber = true;
      updateDisplay();
    } else if (mrcPressCount >= 2) {
      memory = 0;
      mrcPressCount = 0;
      updateDisplay();
    }
  }
};

Calculator calc;

void setup() {
  
  delay(200);
  calc.init();
  
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    
    if (key >= '0' && key <= '9') {
      calc.handleDigit(key);
    }
    else if (key == '+' || key == '-' || key == '*' || key == '/') {
      calc.handleOperation(key);
    }
    else {
      switch(key) {
        case '=':
          calc.calculate();
          break;
        case 'C':
          calc.clear();
          break;
        case '.':
          calc.handleDecimal();
          break;
        case '%':
          calc.percentage();
          break;
        case 'Q':
          calc.squareRoot();
          break;
        case 'N':
          calc.changeSign();
          break;
        case 'A':
          calc.memoryAdd();
          break;
        case 'S':
          calc.memorySubtract();
          break;
        case 'R':
          calc.memoryRecallClear();
          break;
      }
    }
  }
  
  delay(10);
}