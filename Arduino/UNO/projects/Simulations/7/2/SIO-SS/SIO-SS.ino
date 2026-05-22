#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);

String inputString = "";

void setup() {
  mySerial.begin(9600);
  
  mySerial.println("... SoftwareSerial ");
  mySerial.println("Repetovskyi Vladyslav");
  mySerial.println();
}

void loop() {
  while (mySerial.available()) {
    char inChar = (char)mySerial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        processCommand(inputString);
        inputString = "";
      }
    } else {
      inputString += inChar;
    }
  }
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "whois") {
    mySerial.println();
    mySerial.println("Student Information:");
    mySerial.println("--------------------");
    mySerial.println("Name: [Your Name]");
    mySerial.println("Group: [Your Group]");
    mySerial.println("Lab: 7.3 - SoftwareSerial");
    mySerial.println("Pins: RX=10, TX=11");
    mySerial.println();
  }
  else if (cmd == "test") {
    mySerial.println();
    mySerial.println("test:");
    mySerial.println("--------------------");
    
    for(int i = 0; i <= 9; i++) {
      mySerial.print(i);
      mySerial.print(" ");
      delay(50);
    }
    mySerial.println();
    
    for(char c = 'A'; c <= 'Z'; c++) {
      mySerial.print(c);
      delay(30);
    }
    mySerial.println();
    
    byte testData[] = {0x01, 0x02, 0x03, 0x04, 0xFF, 0xFE};
    for(int i = 0; i < 6; i++) {
      mySerial.print("0x");
      if (testData[i] < 0x10) mySerial.print("0");
      mySerial.print(testData[i], HEX);
      mySerial.print(" ");
    }
    mySerial.println();
    mySerial.println();
  }
  else if (cmd.length() > 0) {
    mySerial.print("Unknown command: ");
    mySerial.println(cmd);
    mySerial.println();
  }
}
