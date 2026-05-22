String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(9600);

  Serial.println("...");
  Serial.println();
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        stringComplete = true;
      }
    } else {
      inputString += inChar;
    }
  }
  
  if (stringComplete) {
    processCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "whois") {
    showWhois();
  }
  else if (cmd == "test") {
    sendTestData();
  }
  else if (cmd.length() > 0) {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println();
  }
}

void showWhois() {
  Serial.println();
  Serial.println("info:");
  Serial.println("--------------------");
  Serial.println("Vladyslav Repetovskyi");
  Serial.println("343-B3");
  Serial.println();
}

void sendTestData() {
  Serial.println();
  for(int i = 0; i <= 9; i++) {
    Serial.print(i);
    Serial.print(" ");
    delay(50);
  }
  Serial.println();
  for(char c = 'A'; c <= 'Z'; c++) {
    Serial.print(c);
    delay(30);
  }
  Serial.println();
  
  Serial.print("HEX: ");
  byte testData[] = {0x01, 0x02, 0x03, 0x04, 0xFF, 0xFE};
  for(int i = 0; i < 6; i++) {
    Serial.print("0x");
    if (testData[i] < 0x10) Serial.print("0");
    Serial.print(testData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println();
}