#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


class Display {
  private:
    volatile bool displayOff = false;
    volatile bool displayDim = true;
  public: 
    void init();
    void displayTextMulti(String printArr[], bool ignoreSerial = false);
    void displayTextln(String printStr, bool ignoreSerial = false);
    void displayText(String printStr);
    void displayTextAt(String printStr, int posX, int posY);
    void toggleDim();
    void toggleOff();
};
