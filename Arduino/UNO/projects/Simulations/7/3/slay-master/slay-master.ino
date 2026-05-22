String inputString = "";

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        processMessage(inputString);
        inputString = "";
      }
    } else {
      inputString += inChar;
    }
  }
}
s
void processMessage(String msg) {
  Serial.print("msg: ");
  Serial.println(msg);
  
  if (msg.startsWith("MSG:")) {
    String value = msg.substring(4);
    int num = value.toInt();
    
    Serial.print("ACK:");
    Serial.println(num);
  }
}

