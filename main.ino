#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <GDBStub.h>
#include <SPI.h>
#include <Wire.h>

#include "config.h"
#include "Display.h"
#include "ReportingSensor.h"

AdafruitIO_Feed *temperature = io.feed("home-atmosphere.hometemperature");
AdafruitIO_Feed *humidity = io.feed("home-atmosphere.homehumidity");

#define A_BUTTON 0
#define B_BUTTON 16
#define C_BUTTON 2

Adafruit_BME280 bme;

volatile bool showNetwork = false;
ReportingSensor tempSensor(temperature, -1);
ReportingSensor humiditySensor(humidity);
Display display;

void setup() {
  // INIT: Serial
  Serial.begin(115200);
  
  // INIT: Display
  display.init();

  // INIT: Buttons
  pinMode(A_BUTTON, INPUT_PULLUP);
  pinMode(B_BUTTON, INPUT_PULLUP);
  pinMode(C_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(A_BUTTON), toggleDimDisplay, RISING);
  attachInterrupt(digitalPinToInterrupt(B_BUTTON), toggleDisplay, FALLING);
  attachInterrupt(digitalPinToInterrupt(C_BUTTON), revealLocalNetwork, RISING);

  // INIT: Sensors
  display.displayTextln("INIT: BME280: Starting...");
  if (!bme.begin()) {
      display.displayTextln("HANG: Could not find BME280 sensor!");
      Serial.println("SensorID was: 0x" + (bme.sensorID(), 16));
      while (1) delay(2000);
  }
  
  tempSensor.setMonitor([](){ 
    float temp = bme.readTemperature(); 
    temp = temp * 1.8 + 32; // C -> F
    return temp;
  });
  
  humiditySensor.setMonitor([]() {
    return bme.readHumidity();
  });
  
  display.displayTextln("INIT: BME280: Complete");
  
  // INIT: Adafruit.io
  display.displayTextln("");
  display.displayText("INIT: AdaIO: Connecting");
  io.connect();
  while(io.status() < AIO_CONNECTED) {
    display.displayText(".");
    delay(500);
  }

  display.displayTextln("");
  display.displayTextln("INIT: AdaIO: Complete");
}

void loop() {
  io.run();
  
  if(showNetwork) {
      printLocalNetwork();
      delay(4000);
      showNetwork = false;
  }
  
  // read & report sensors
  float pressureReading = bme.readPressure() / 100.0F;
  ReportResult temperatureResult = tempSensor.readAndReport();
  ReportResult humidityResult = humiditySensor.readAndReport();

  String output[3] = {
    "Temperature: " + String(temperatureResult.value) + " F",
    "Pressure: " + String(pressureReading) +"hPa\n",
    "Humidity: " + String(humidityResult.value) + "%\n"
  };

  display.displayTextMulti(output, false);

  if(temperatureResult.inBurnin) {
    display.displayTextAt("(B)", 90, 24);
  }

  if(temperatureResult.inCooldown || humidityResult.inCooldown) {
    display.displayTextAt("(C)", 108, 24);
  }

  delay(1000);
}

void printLocalNetwork() {
  display.displayTextln("SSID: " + WiFi.SSID());
  display.displayText("IP: " + WiFi.localIP().toString() + "\n");
  display.displayText("Status: " + String(WiFi.status()) + " / " + String(WiFi.getMode()));
}

ICACHE_RAM_ATTR void revealLocalNetwork() {
  showNetwork = true;
}

ICACHE_RAM_ATTR void toggleDisplay() {
  Serial.println("Toggling thing");
  display.toggleOff();
}

ICACHE_RAM_ATTR void toggleDimDisplay() {
  display.toggleDim();
}
