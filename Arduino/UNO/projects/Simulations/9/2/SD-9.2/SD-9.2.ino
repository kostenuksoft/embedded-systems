#include <SD.h>
#include <SPI.h>

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
    Serial.println("Initializing SD card");
    return true;
  }
  
  bool writeToFile(const char* filename, const char* data) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
      Serial.print("Writing to ");
      Serial.print(filename);
      Serial.println(":");
      file.println(data);
      file.close();
      Serial.println(data);
      return true;
    }
    return false;
  }
  
  void readFromFile(const char* filename) {
    File file = SD.open(filename);
    if (file) {
      Serial.print("Reading from ");
      Serial.print(filename);
      Serial.println(":");
      while (file.available()) {
        Serial.write(file.read());
      }
      file.close();
    }
  }
};

SDCardHandler sdCard(10);

void setup() {
  Serial.begin(9600);
  
  if (!sdCard.initialize()) {
    return;
  }
  
  sdCard.writeToFile("test.txt", "testing 1, 2, 3.");
  sdCard.readFromFile("test.txt");
}

void loop() {
}