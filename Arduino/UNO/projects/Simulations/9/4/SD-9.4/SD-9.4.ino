#include <SD.h>
#include <SPI.h>
#include <Wire.h>

class SDCardHandler {
private:
  const int csPin;
  
public:
  SDCardHandler(int cs) : csPin(cs) {}
  
  bool initialize() {
    if (!SD.begin(csPin)) {
      Serial.println("SD Card failed");
      return false;
    }
    Serial.println("SD card initialized");
    return true;
  }
  
  bool writeToFile(const char* filename, const char* data) {
    if (SD.exists(filename)) {
      SD.remove(filename);
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      file.print(data);
      file.close();
      return true;
    }
    return false;
  }
  
  String readFromFile(const char* filename) {
    String content = "";
    File file = SD.open(filename, FILE_READ);
    if (file) {
      while (file.available()) {
        content += (char)file.read();
      }
      file.close();
    }
    return content;
  }
};

class EEPROMHandler {
private:
  const int deviceAddress = 0x50;
  
public:
  void begin() {
    Wire.begin();
  }
  
  void writeByte(unsigned int address, byte data) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((byte)(address >> 8));
    Wire.write((byte)(address & 0xFF));
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
  }
  
  void writeString(unsigned int address, String data) {
    for (unsigned int i = 0; i < data.length(); i++) {
      writeByte(address + i, data[i]);
    }
    writeByte(address + data.length(), '\0');
  }
  
  byte readByte(unsigned int address) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((byte)(address >> 8));
    Wire.write((byte)(address & 0xFF));
    Wire.endTransmission();
    
    Wire.requestFrom(deviceAddress, 1);
    if (Wire.available()) {
      return Wire.read();
    }
    return 0xFF;
  }
  
  String readString(unsigned int address, int maxLength) {
    String result = "";
    for (int i = 0; i < maxLength; i++) {
      byte data = readByte(address + i);
      if (data == 0 || data == 0xFF) break;
      result += (char)data;
    }
    return result;
  }
};

SDCardHandler sdCard(10);
EEPROMHandler eeprom;

void setup() {
  Serial.begin(9600);
  
  if (!sdCard.initialize()) {
    return;
  }
  
  eeprom.begin();
  
  String testData = "Hello from SD card..";
  
  Serial.println("Writing to SD card");
  sdCard.writeToFile("data.txt", testData.c_str());
  
  Serial.println("Reading from SD card");
  String dataFromSD = sdCard.readFromFile("data.txt");
  Serial.print("Data from SD: ");
  Serial.println(dataFromSD);
  
  Serial.println("Writing to EEPROM");
  eeprom.writeString(0, dataFromSD);
  
  Serial.println("Reading from EEPROM");
  String dataFromEEPROM = eeprom.readString(0, 50);
  Serial.print("Data from EEPROM: ");
  Serial.println(dataFromEEPROM);
}

void loop() {
}