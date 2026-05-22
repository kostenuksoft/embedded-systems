#include <Keypad.h>
#include <LiquidCrystal.h>

class Display {
private:
  LiquidCrystal lcd;
  
public:
  Display(int rs, int enable, int d4, int d5, int d6, int d7) 
    : lcd(rs, enable, d4, d5, d6, d7) {
  }
  
  void init() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calculator Ready");
    delay(1000);
    clear();
    showNumber(0);
  }
  
  void clear() {
    lcd.clear();
  }
  
  void showNumber(float number) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    if (number == (int)number) {
      lcd.print((long)number);
    } else {
      lcd.print(number, 2);
    }
  }
  
  void showExpression(float num1, char op, float num2, float result) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (num1 == (int)num1) lcd.print((long)num1);
    else lcd.print(num1, 2);
    
    lcd.print(" ");
    lcd.print(op);
    lcd.print(" ");
    
    if (num2 == (int)num2) lcd.print((long)num2);
    else lcd.print(num2, 2);
    
    lcd.setCursor(0, 1);
    lcd.print("= ");
    if (result == (int)result) lcd.print((long)result);
    else lcd.print(result, 2);
  }
  
  void showError(String message) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error:");
    lcd.setCursor(0, 1);
    lcd.print(message);
  }
};

class Calculator {
private:
  float firstNumber;
  float secondNumber;
  char operation;
  bool operationPressed;
  bool calculationDone;
  Display* display;
  
public:
  Calculator(Display* disp) : display(disp) {
    reset();
  }
  
  void reset() {
    firstNumber = 0;
    secondNumber = 0;
    operation = ' ';
    operationPressed = false;
    calculationDone = false;
    display->clear();
    display->showNumber(0);
  }
  
  void inputNumber(int digit) {
    if (calculationDone) {
      firstNumber = digit;
      calculationDone = false;
      operationPressed = false;
    }
    else if (!operationPressed) {
      if (firstNumber < 99999999) {
        firstNumber = firstNumber * 10 + digit;
      }
    }
    else {
      if (secondNumber < 99999999) {
        secondNumber = secondNumber * 10 + digit;
      }
    }
    
    float currentNumber = operationPressed ? secondNumber : firstNumber;
    display->showNumber(currentNumber);
  }
  
  void inputOperation(char op) {
    if (operationPressed && secondNumber != 0) {
      calculate();
    }
    
    operation = op;
    operationPressed = true;
    secondNumber = 0;
    
    display->clear();
    display->showNumber(firstNumber);
  }
  
  void calculate() {
    if (!operationPressed) return;
    
    float result = 0;
    bool validOperation = true;
    
    switch(operation) {
      case '+': 
        result = firstNumber + secondNumber;
        break;
      case '-': 
        result = firstNumber - secondNumber;
        break;
      case 'x': 
        result = firstNumber * secondNumber;
        break;
      case '/': 
        if (secondNumber != 0) {
          result = firstNumber / secondNumber;
        } else {
          display->showError("Div by zero!");
          delay(2000);
          reset();
          return;
        }
        break;
      default:
        validOperation = false;
        break;
    }
    
    if (validOperation) {
      display->showExpression(firstNumber, operation, secondNumber, result);
      firstNumber = result;
      secondNumber = 0;
      operationPressed = false;
      calculationDone = true;
    }
  }
  
  float getCurrentNumber() {
    return operationPressed ? secondNumber : firstNumber;
  }
};

class KeypadHandler {
private:
  static const byte ROWS = 4;
  static const byte COLS = 4;
  
  char keys[ROWS][COLS] = {
    {'7','8','9','/'},
    {'4','5','6','x'},
    {'1','2','3','-'},
    {'*','0','=','+'}
  };
  
  byte rowPins[ROWS] = {9, 8, 7, 6}; 
  byte colPins[COLS] = {5, 4, 3, 2}; 
  
  Keypad keypad;
  Calculator* calculator;
  
public:
  KeypadHandler(Calculator* calc) 
    : keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS), calculator(calc) {
  }
  
  void processInput() {
    char key = keypad.getKey();
    
    if (key) {
      if (key >= '0' && key <= '9') {
        int digit = key - '0';
        calculator->inputNumber(digit);
      }
      else if (key == '+' || key == '-' || key == 'x' || key == '/') {
        calculator->inputOperation(key);
      }
      else if (key == '=') {
        calculator->calculate();
      }
      else if (key == '*') {
        calculator->reset();
      }
    }
  }
};

Display display(12, 11, A0, A1, A2, A3); 
Calculator calculator(&display);
KeypadHandler keypadHandler(&calculator);

void setup() {
  display.init();
}

void loop() {
  keypadHandler.processInput();
}