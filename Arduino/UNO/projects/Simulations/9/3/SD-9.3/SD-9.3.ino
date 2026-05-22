#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class SDCardHandler {
private:
  const int csPin;
  
public:
  SDCardHandler(int cs) : csPin(cs) {}
  
  bool initialize() {
    if (!SD.begin(csPin)) {
      Serial.println("Card failed");
      return false;
    }
    Serial.println("SD card initialized");
    return true;
  }
  
  bool appendToFile(const char* filename, String data) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.println(data);
      file.close();
      return true;
    }
    return false;
  }
};

class TemperatureSensor {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  
public:
  TemperatureSensor(int pin) : oneWire(pin), sensors(&oneWire) {}
  
  void begin() {
    sensors.begin();
    sensors.setResolution(12);
    sensors.setWaitForConversion(true);
  }
  
  float readTemperature() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    if (temp == -127.00 || temp == 85.00) {
      delay(100);
      sensors.requestTemperatures();
      temp = sensors.getTempCByIndex(0);
    }
    
    return temp;
  }
  
  bool isValidTemperature(float temp) {
    return (temp != -127.00 && temp != 85.00);
  }
};

SDCardHandler sdCard(10);
TemperatureSensor tempSensor(3);

void setup() {
  Serial.begin(9600);
  
  if (!sdCard.initialize()) {
    return;
  }
  
  tempSensor.begin();
  delay(1000);
}

void loop() {
  float temperature = tempSensor.readTemperature();
  
  if (tempSensor.isValidTemperature(temperature)) {
    String data = "Temperature: " + String(temperature, 2) + " C";
    Serial.println(data);
    sdCard.appendToFile("temp.txt", data);
  } else {
    Serial.println("Sensor error");
  }
  
  delay(500);
}