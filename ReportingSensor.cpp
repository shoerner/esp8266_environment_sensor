#include "AdafruitIO_WiFi.h"
#include "ReportingSensor.h"

#define SENSOR_BURNIN_WAIT 1800000
#define MAX_BOUNDS 120

ReportingSensor::ReportingSensor(AdafruitIO_Feed *feed, int offset) {
  this->feed = feed;
  this->lastValue = 0;
  this->lastReported = millis();
  this->nextReporting = millis() + SENSOR_BURNIN_WAIT;
  this->cooldown = this->nextReporting;
  this->temperatureOffset = offset;
  this->inBurnin = true;
}

ReportingSensor::ReportingSensor(AdafruitIO_Feed *feed, std::function<float()> func, int offset) {
  this->feed = feed;
  this->lastValue = 0;
  this->lastReported = millis();
  this->nextReporting = millis() + SENSOR_BURNIN_WAIT;
  this->cooldown = this->nextReporting;
  this->temperatureOffset = offset;
  this->monitorFunction = func;
  this->inBurnin = true;
}

void ReportingSensor::setMonitor(std::function<int()> func) {
  this->monitorFunction = func;
}

ReportResult ReportingSensor::readAndReport() {
  int temperature = monitorFunction() + temperatureOffset;
  bool reportVal = report(temperature);
  ReportResult res;
  res.value = temperature;
  res.reportStatus = reportVal;
  res.inBurnin = inBurnin;
  res.inCooldown = !inBurnin && cooldown > millis();
  return res;
}

bool ReportingSensor::report(int value) {
  bool timeElapsed = millis() > nextReporting;
  bool inCooldown = millis() <= cooldown;

  // Make sure the value is sane
  if (value > MAX_BOUNDS && !timeElapsed) {
    return false;
  }

  // Don't tell Adafruit the same information 2x
  // UNLESS it's been a hot minute.
  if (inBurnin || (value == lastValue && !timeElapsed) || inCooldown) {
    if (inCooldown && !inBurnin)
      Serial.println("Cooldown: force ignoring changes (Timeout: "  + String((cooldown - millis()) / 1000) + "s)");
    if (!timeElapsed)
      Serial.println("No change and update not timed out (Timeout: "  + String((nextReporting - millis()) / 1000) + "s)");
    if (inBurnin) {
      inBurnin = !timeElapsed;
      Serial.println("In burn-in, new state: " + String(inBurnin));
      lastValue = value;
    }

    Serial.println("Not reporting changes to adafruit.");
    return true;
  }

  if (timeElapsed)
    Serial.println("Timeout for sensor reporting. Report forced.");

  Serial.println("Difference: " + String(lastValue) + " -> " + String(value));

  bool reportStatus = feed->save(value);

  lastValue = value;
  lastReported = millis();
  nextReporting = millis() + (5 * 60 * 1000);
  cooldown = millis() + 3000;

  Serial.println("Last communication result: " + String(reportStatus));
  Serial.println("Next checkin: " + String((nextReporting - millis()) / 1000) + "s");

  return reportStatus;
}
