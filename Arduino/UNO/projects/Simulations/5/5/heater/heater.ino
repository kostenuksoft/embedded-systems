#include <Arduino.h>
#include <LiquidCrystal.h>

#define TEMP_SENSOR A0  
#define HEATER_PWM 6    

#define KEYPAD_ROW1 13
#define KEYPAD_ROW2 10
#define KEYPAD_ROW3 9
#define KEYPAD_ROW4 8

#define KEYPAD_COL1 A3
#define KEYPAD_COL2 A2
#define KEYPAD_COL3 A1

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

class Keypad3x4 {
private:
  uint8_t rowPins[4] = {KEYPAD_ROW1, KEYPAD_ROW2, KEYPAD_ROW3, KEYPAD_ROW4};
  uint8_t colPins[3] = {KEYPAD_COL1, KEYPAD_COL2, KEYPAD_COL3};
  
  char keys[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'R', '0', 'S'}  
  };
  
public:
  void begin() {
    for (uint8_t i = 0; i < 4; i++) {
      pinMode(rowPins[i], OUTPUT);
      digitalWrite(rowPins[i], HIGH);
    }
    
    for (uint8_t i = 0; i < 3; i++) {
      pinMode(colPins[i], INPUT_PULLUP);
    }
  }
  
  char getKey() {
    for (uint8_t row = 0; row < 4; row++) {
      digitalWrite(rowPins[row], LOW);
      delayMicroseconds(10);
      
      for (uint8_t col = 0; col < 3; col++) {
        if (digitalRead(colPins[col]) == LOW) {
          digitalWrite(rowPins[row], HIGH);
          delay(200); 
          return keys[row][col];
        }
      }
      
      digitalWrite(rowPins[row], HIGH);
    }
    
    return '\0'; 
  }
};

class TemperatureSensor {
private:
  uint8_t pin;
  float vref;
  
public:
  TemperatureSensor(uint8_t sensorPin, float referenceVoltage = 5.0) 
    : pin(sensorPin), vref(referenceVoltage) {}
  
  void begin() {
    pinMode(pin, INPUT);
    analogReference(DEFAULT);
  }
  
  float readVoltage() {
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += analogRead(pin);
      delay(2);
    }
    uint16_t raw = sum / 10;
    return (raw / 1023.0) * vref;
  }
  
  float readTemperature() {
    float voltage = readVoltage();
    float temperature = voltage * 20.0; 
    return constrain(temperature, 0, 150);
  }
};

class HeaterController {
private:
  uint8_t pwmPin;
  uint8_t currentDuty;
  
public:
  HeaterController(uint8_t pin) : pwmPin(pin), currentDuty(0) {}
  
  void begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);
  }
  
  void setPowerPercent(float percent) {
    percent = constrain(percent, 0.0, 100.0);
    currentDuty = (uint8_t)((percent / 100.0) * 255);
    analogWrite(pwmPin, currentDuty);
  }
  
  uint8_t getPowerPercent() const {
    return (uint8_t)((currentDuty / 255.0) * 100.0);
  }
  
  void off() {
    currentDuty = 0;
    analogWrite(pwmPin, 0);
  }
};

class PIDController {
private:
  float kp, ki, kd;
  float integral;
  float lastError;
  unsigned long lastTime;
  
public:
  PIDController(float p, float i, float d) 
    : kp(p), ki(i), kd(d), integral(0), lastError(0), lastTime(0) {}
  
  float compute(float setpoint, float current) {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;  
    
    if (dt < 0.1) return 0; 
    
    float error = setpoint - current;
    float P = kp * error;
    
    integral += error * dt;
    integral = constrain(integral, -100, 100);
    float I = ki * integral;
    
    float derivative = (error - lastError) / dt;
    float D = kd * derivative;
    
    lastError = error;
    lastTime = now;
    
    float output = P + I + D;
    return constrain(output, 0, 100);
  }
  
  void reset() {
    integral = 0;
    lastError = 0;
    lastTime = millis();
  }
};

TemperatureSensor tempSensor(TEMP_SENSOR);
HeaterController heater(HEATER_PWM);
Keypad3x4 keypad;
PIDController pid(5.0, 0.5, 1.0); 

float setpoint = 0.0;
float currentTemp = 0.0;
uint8_t heaterPower = 0;
String inputBuffer = "";
bool settingTemp = false;

void setup() {
  
  tempSensor.begin();
  heater.begin();
  keypad.begin();
  lcd.begin(16, 2);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1000);
  pid.reset();
  updateDisplay();
  
}

void loop() {
  static unsigned long lastControl = 0;
  static unsigned long lastDisplay = 0;
  
  if (millis() - lastControl >= 500) {
    lastControl = millis();
    currentTemp = tempSensor.readTemperature();
    heaterPower = (uint8_t) pid.compute(setpoint, currentTemp);    
    heater.setPowerPercent(heaterPower);
  }
  
  if (millis() - lastDisplay >= 500) {
    lastDisplay = millis();
    if (!settingTemp) {
      updateDisplay();
    }
  }
  
  char key = keypad.getKey();
  if (key != '\0') {
    handleKey(key);
  }
}

void handleKey(char key) {
  
  if (key == 'S') {
    if (!settingTemp) {
      settingTemp = true;
      inputBuffer = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set Target Temp:");
      lcd.setCursor(0, 1);
      lcd.print("> ");
      lcd.cursor();
    } else {
      if (inputBuffer.length() > 0) {
        float newSetpoint = inputBuffer.toFloat();
        if (newSetpoint >= 0 && newSetpoint <= 100) {
          setpoint = newSetpoint;
          pid.reset();  
        }
      }
      settingTemp = false;
      inputBuffer = "";
      lcd.noCursor();
      updateDisplay();
    }
  }
  else if (key == 'R') {
    settingTemp = false;
    inputBuffer = "";
    heater.off();
    heaterPower = 0;
    pid.reset();
    lcd.noCursor();
    updateDisplay();
  }
  else if (key >= '0' && key <= '9') {
    if (settingTemp && inputBuffer.length() < 3) {
      inputBuffer += key;
      lcd.setCursor(2 + inputBuffer.length() - 1, 1);
      lcd.print(key);
    }
  }
}

void updateDisplay() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(currentTemp, 1);
  lcd.print((char)223); 
  lcd.print("C");
  
  if (abs(currentTemp - setpoint) < 2.0) {
    lcd.setCursor(15, 0);
    lcd.print("*");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.print(setpoint, 0);
  lcd.print((char)223);
  lcd.print("C ");
  
  lcd.print("H:");
  lcd.print(heaterPower);
  lcd.print("%");
}
