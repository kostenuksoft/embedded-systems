#include <Keypad.h>
#include <LedControl.h>
#include <math.h>

const byte ROWS = 4;
const byte COLS = 6; 
char keys[ROWS][COLS] = {
  {'/', 'X', '9', '8', '7', 'A'},
  {'R', '-', '6', '5', '4', 'B'},
  {'M', '+', '3', '2', '1', 'C'},
  {'N', '+', '=', '.', '0', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2, 1, 0};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;
const int MAX_DISPLAYS = 4; 

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MAX_DISPLAYS);

double firstNumber = 0;
double secondNumber = 0;
double result = 0;
char operation = 0;
bool enteringSecondNumber = false;
bool calculationDone = false;
double memory = 0;

String displayText = "0";
int displayPosition = 0;

void setup() {
  Serial.begin(9600);
  
 
  for (int i = 0; i < MAX_DISPLAYS; i++) {
    lc.shutdown(i, false);      
    lc.setIntensity(i, 8);      
    lc.clearDisplay(i);         
  }
  
  updateDisplay();
  
  Serial.println("Extended Calculator Ready!");
  Serial.println("Keys: 0-9=numbers, +=add, -=subtract, X=multiply, /=divide");
  Serial.println("A=ON/C, B=+/-, C=%, D=√, R=MRC, M=M-, N=M+");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    handleKeypress(key);
  }
}

void handleKeypress(char key) {
  switch (key) {
    case '0'...'9':
      handleNumber(key);
      break;
    case '.':
      handleDecimal();
      break;
    case '+':
    case '-':
    case 'X':
    case '/':
      handleOperation(key);
      break;
    case '=':
      handleEquals();
      break;
    case 'A': // ON/C - Clear
      handleClear();
      break;
    case 'B': // +/- - Change sign
      handleSign();
      break;
    case 'C': // % - Percentage
      handlePercent();
      break;
    case 'D': // sqrt
      handleSquareRoot();
      break;
    case 'R': // MRC - Memory Recall/Clear
      handleMemoryRecall();
      break;
    case 'M': // M- - Memory Subtract
      handleMemorySubtract();
      break;
    case 'N': // M+ - Memory Add
      handleMemoryAdd();
      break;
  }
}

void handleNumber(char key) {
  if (calculationDone) {
    displayText = "";
    calculationDone = false;
  }
  
  if (displayText == "0" && !enteringSecondNumber) {
    displayText = "";
  }
  
  if (displayText.length() < 8) { 
    displayText += key;
  }
  
  updateDisplay();
}

void handleDecimal() {
  if (calculationDone) {
    displayText = "0";
    calculationDone = false;
  }
  
  if (displayText.indexOf('.') == -1) {
    if (displayText == "" || enteringSecondNumber) {
      displayText = "0.";
    } else {
      displayText += ".";
    }
  }
  
  updateDisplay();
}

void handleOperation(char op) {
  if (!calculationDone && operation != 0) {
    handleEquals(); 
  }
  
  firstNumber = displayText.toDouble();
  operation = op;
  enteringSecondNumber = true;
  displayText = "";
}

void handleEquals() {
  if (operation == 0) return;
  
  secondNumber = displayText.toDouble();
  
  switch (operation) {
    case '+':
      result = firstNumber + secondNumber;
      break;
    case '-':
      result = firstNumber - secondNumber;
      break;
    case 'X':
      result = firstNumber * secondNumber;
      break;
    case '/':
      if (secondNumber != 0) {
        result = firstNumber / secondNumber;
      } else {
        displayText = "Error";
        updateDisplay();
        return;
      }
      break;
  }
  
  displayText = formatNumber(result);
  operation = 0;
  enteringSecondNumber = false;
  calculationDone = true;
  
  updateDisplay();
}

void handleClear() {
  firstNumber = 0;
  secondNumber = 0;
  result = 0;
  operation = 0;
  enteringSecondNumber = false;
  calculationDone = false;
  displayText = "0";
  
  updateDisplay();
}

void handleSign() {
  double num = displayText.toDouble();
  num = -num;
  displayText = formatNumber(num);
  updateDisplay();
}

void handlePercent() {
  double num = displayText.toDouble();
  num = num / 100.0;
  displayText = formatNumber(num);
  calculationDone = true;
  updateDisplay();
}

void handleSquareRoot() {
  double num = displayText.toDouble();
  if (num >= 0) {
    num = sqrt(num);
    displayText = formatNumber(num);
    calculationDone = true;
    updateDisplay();
  } else {
    displayText = "Error";
    updateDisplay();
  }
}

void handleMemoryRecall() {
  displayText = formatNumber(memory);
  calculationDone = true;
  updateDisplay();
}

void handleMemoryAdd() {
  memory += displayText.toDouble();
  Serial.print("Memory: ");
  Serial.println(memory);
}

void handleMemorySubtract() {
  memory -= displayText.toDouble();
  Serial.print("Memory: ");
  Serial.println(memory);
}

String formatNumber(double num) {
  if (isnan(num) || isinf(num)) {
    return "Error";
  }
 
  String result;
  if (abs(num) < 0.0001 && num != 0) {
    result = String(num, 6); 
  } else if (abs(num) >= 99999999) {
    result = "Overflow";
  } else {
    result = String(num, 6);
   
    if (result.indexOf('.') != -1) {
      while (result.endsWith("0") && !result.endsWith(".0")) {
        result.remove(result.length() - 1);
      }
      if (result.endsWith(".")) {
        result.remove(result.length() - 1);
      }
    }
  }
  
  if (result.length() > 8) {
    result = result.substring(0, 8);
  }
  
  return result;
}

void updateDisplay() {
  for (int i = 0; i < MAX_DISPLAYS; i++) {
    lc.clearDisplay(i);
  }
  
  String text = displayText;
  
  if (text == "Error" || text == "Overflow") {
    displayError(text);
    return;
  }
  
  while (text.length() < 8) {
    text = " " + text;
  }
  
  for (int i = 0; i < min(8, (int)text.length()); i++) {
    char c = text.charAt(7 - i); 
    int displayIndex = i / 2;
    int digitPosition = (i % 2) ? 0 : 1;
    
    displayCharacter(displayIndex, digitPosition, c);
  }
}

void displayCharacter(int display, int digit, char c) {
  byte pattern = 0;
  
  switch (c) {
    case '0': pattern = B01111110; break;
    case '1': pattern = B00110000; break;
    case '2': pattern = B01101101; break;
    case '3': pattern = B01111001; break;
    case '4': pattern = B00110011; break;
    case '5': pattern = B01011011; break;
    case '6': pattern = B01011111; break;
    case '7': pattern = B01110000; break;
    case '8': pattern = B01111111; break;
    case '9': pattern = B01111011; break;
    case '.': pattern = B10000000; break;
    case '-': pattern = B00000001; break;
    case 'E': pattern = B01001111; break; 
    case 'r': pattern = B00001101; break;
    case 'o': pattern = B00011101; break;
    case 'F': pattern = B01000111; break; 
    case 'L': pattern = B00001110; break;
    case ' ': pattern = B00000000; break;
    default: pattern = B00000001; break; 
  }
  
  lc.setRow(display, digit, pattern);
}

void displayError(String errorMsg) {
  lc.setRow(0, 1, B01001111); 
  lc.setRow(0, 0, B00001101); 
  lc.setRow(1, 1, B00001101); 
  
  if (errorMsg == "Overflow") {
    lc.setRow(1, 0, B01000111); 
    lc.setRow(2, 1, B00001110); 
  }
}