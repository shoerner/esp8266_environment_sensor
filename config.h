/************************ Adafruit IO Config *******************************/

#define IO_USERNAME "YOUR_ADAFRUIT_USER"
#define IO_KEY "YOUR_ADAFRUIT_KEY"

/******************************* WiFi **************************************/

#include "AdafruitIO_WiFi.h"
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
