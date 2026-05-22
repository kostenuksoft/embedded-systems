String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Serial.println("...");
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        stringComplete = true;
      }
    } 
    else {
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
  else if (cmd == "led on") {
    digitalWrite(13, HIGH);
    Serial.println("LED: ON");
    Serial.println();
  }
  else if (cmd == "led off") {
    digitalWrite(13, LOW);
    Serial.println("LED: OFF");
    Serial.println();
  }
  else if (cmd == "blink") {
    blinkLED();
  }
  else if (cmd == "status") {
    showStatus();
  }
  else if (cmd.length() > 0) {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println();
  }
}

void showWhois() {
  Serial.println();
  Serial.println("Repetovskyi Vladyslav");
  Serial.println("343-B3");
  Serial.println();
}

void blinkLED() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(13, HIGH);
    Serial.print(".");
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
  
  Serial.println("ok.");
  Serial.println();
}

void showStatus() {
  Serial.println();
  Serial.print("up: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.print("LED: ");
  Serial.println(digitalRead(13) ? "ON" : "OFF");
  Serial.println();
}
