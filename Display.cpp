#include "Display.h"

#define OLED_RESET 3
Adafruit_SSD1306 oledDisplay(128, 32, &Wire, OLED_RESET);

void Display::init() {
    oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    oledDisplay.setTextSize(1);
    oledDisplay.setTextColor(WHITE);
    oledDisplay.dim(this->displayDim);
    oledDisplay.clearDisplay();

    this->displayTextln("INIT: Display: Complete");
}

void Display::displayTextMulti(String printArr[], bool ignoreSerial) {
    if(this->displayOff) {
      return; 
    }
    
    oledDisplay.clearDisplay();

    for (int i = 0; i < sizeof(printArr); i++)
    {
        printArr[i].trim();
        if (!ignoreSerial || this->displayOff)
            Serial.println(printArr[i]);

        oledDisplay.setCursor(0, i * 8);
        oledDisplay.print(printArr[i]);
    }

    oledDisplay.display();
}

void Display::displayTextln(String printStr, bool ignoreSerial) {
    if (!ignoreSerial || this->displayOff)
    {
        Serial.println(printStr);
    }

    if(!this->displayOff) {
      oledDisplay.clearDisplay();
      oledDisplay.setCursor(0, 0);
      oledDisplay.println(printStr);
      oledDisplay.display();
    }
}

void Display::displayText(String printStr) {
    Serial.print(printStr);

    if(!this->displayOff) {
      oledDisplay.print(printStr);
      oledDisplay.display();
    }
}

void Display::displayTextAt(String printStr, int posX, int posY) {
    if(!this->displayOff) {
      oledDisplay.setCursor(posX, posY);
      oledDisplay.print(printStr);
      oledDisplay.display();
    }
}

void Display::toggleDim() {
  this->displayDim = !this->displayDim;

  oledDisplay.dim(this->displayDim);
}

void Display::toggleOff() {
  this->displayOff = !this->displayOff;

  if(this->displayOff) {
    Serial.println("Clearing display for 'off' state");
    //oledDisplay.clearDisplay();
  }
}
