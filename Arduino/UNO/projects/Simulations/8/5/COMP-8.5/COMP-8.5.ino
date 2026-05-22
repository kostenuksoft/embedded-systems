#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t LCD1_ADDRESS = 0x20;
const uint8_t LCD2_ADDRESS = 0x21;

class I2CDisplay {
private:
    LiquidCrystal_I2C lcd;
    uint8_t address;

public:
    I2CDisplay(uint8_t addr) : lcd(addr, 16, 2), address(addr) {}

    void begin() {
        lcd.init();
        lcd.backlight();
    }

    void clear() {
        lcd.clear();
    }

    void print(const char* text, uint8_t col = 0, uint8_t row = 0) {
        lcd.setCursor(col, row);
        lcd.print(text);
    }

    void print(int value, uint8_t col = 0, uint8_t row = 0) {
        lcd.setCursor(col, row);
        lcd.print(value);
    }

    void print(float value, uint8_t decimals, uint8_t col = 0, uint8_t row = 0) {
        lcd.setCursor(col, row);
        lcd.print(value, decimals);
    }
};

class MultiDisplayController {
private:
    I2CDisplay* displays[2];
    uint8_t displayCount;

public:
    MultiDisplayController(uint8_t addr1, uint8_t addr2) : displayCount(2) {
        displays[0] = new I2CDisplay(addr1);
        displays[1] = new I2CDisplay(addr2);
    }

    ~MultiDisplayController() {
        for (uint8_t i = 0; i < displayCount; i++) {
            delete displays[i];
        }
    }

    void begin() {
        Wire.begin();
        for (uint8_t i = 0; i < displayCount; i++) {
            displays[i]->begin();
        }
    }

    I2CDisplay* getDisplay(uint8_t index) {
        if (index < displayCount) {
            return displays[index];
        }
        return nullptr;
    }

    void clearAll() {
        for (uint8_t i = 0; i < displayCount; i++) {
            displays[i]->clear();
        }
    }

    void displayMessage(uint8_t displayIndex, const char* line1, const char* line2 = "") {
        if (displayIndex < displayCount) {
            displays[displayIndex]->clear();
            displays[displayIndex]->print(line1, 0, 0);
            if (line2[0] != '\0') {
                displays[displayIndex]->print(line2, 0, 1);
            }
        }
    }
};

class MessageManager {
private:
    MultiDisplayController* controller;
    unsigned long lastUpdate;
    unsigned long updateInterval;
    uint8_t messageIndex;

public:
    MessageManager(MultiDisplayController* ctrl, unsigned long interval = 2000) 
        : controller(ctrl), lastUpdate(0), updateInterval(interval), messageIndex(0) {}

    void begin() {
        controller->begin();
        delay(500);
        controller->displayMessage(0, "Hello LCD 1");
        controller->displayMessage(1, "Hello LCD 2");
    }

    void update() {
        unsigned long currentTime = millis();
        
        if (currentTime - lastUpdate >= updateInterval) {
            switch (messageIndex) {
                case 0:
                    controller->displayMessage(0, "Hello LCD 1");
                    controller->displayMessage(1, "Hello LCD 2");
                    break;
                case 1:
                    controller->displayMessage(0, "Display 1", "I2C: 0x20");
                    controller->displayMessage(1, "Display 2", "I2C: 0x21");
                    break;
                case 2:
                    controller->displayMessage(0, "Arduino UNO", "Multi Display");
                    controller->displayMessage(1, "I2C Protocol", "Demo");
                    break;
            }
            
            messageIndex = (messageIndex + 1) % 3;
            lastUpdate = currentTime;
        }
    }
};

MultiDisplayController* displayController;
MessageManager* messageManager;

void setup() {
    displayController = new MultiDisplayController(LCD1_ADDRESS, LCD2_ADDRESS);
    messageManager = new MessageManager(displayController, 3000);
    
    messageManager->begin();
}

void loop() {
    messageManager->update();
}

