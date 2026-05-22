#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

const uint8_t ONE_WIRE_PIN = 2;

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(7, 6, 8, 9, 10, 11);

void setup() {
    lcd.begin(16, 2);
    sensors.begin();
    delay(1000);
}

void loop() {
    sensors.requestTemperatures();
    delay(1000);
    
    float temp = sensors.getTempCByIndex(0);
    
    if(temp == -127 || temp == 85){
        lcd.setCursor(0, 1);
        lcd.print("Reading...");
    }
    else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temperature:");
        lcd.setCursor(0, 1);
        lcd.print(temp, 2);
        lcd.print(" C");
    }
    delay(1000);
}