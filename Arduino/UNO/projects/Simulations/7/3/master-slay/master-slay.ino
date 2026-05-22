void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long lastSend = 0;
  static int counter = 0;
  
  if (millis() - lastSend >= 2000) {
    lastSend = millis();
    
    Serial.print("MSG:");
    Serial.print(counter);
    Serial.println();
    
    counter++;
    
    if (counter > 99) counter = 0;
  }
  
  if (Serial.available()) {
    String response = "";
    while (Serial.available()) {
      char c = Serial.read();
      if (c != '\n' && c != '\r') {
        response += c;
      }
      delay(2);
    }
    
    if (response.length() > 0) {
      Serial.print("slave: ");
      Serial.println(response);
    }
  }
}

