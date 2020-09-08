#include "AdafruitIO_WiFi.h"
#include "ReportResult.h"

class ReportingSensor {
  private:
    unsigned long lastReported;
    unsigned long nextReporting;
    unsigned long cooldown;
    int lastValue;
    AdafruitIO_Feed *feed; 
    int temperatureOffset;
    std::function<int()> monitorFunction;
    bool inBurnin;
  public:
    ReportingSensor(AdafruitIO_Feed *feed, int offset=0);
    ReportingSensor(AdafruitIO_Feed *feed, std::function<float()> func, int offset=0);

    bool report(int value);
    ReportResult readAndReport();
    void setMonitor(std::function<int()> func);
};
