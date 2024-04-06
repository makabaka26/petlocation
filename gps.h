// GPS.h
#ifndef GPS_h
#define GPS_h

#include <Arduino.h>
#include <TinyGPS++.h>

class SoftwareSerial;

class GPS {
  public:
    GPS(uint8_t rxPin, uint8_t txPin, uint32_t baudRate);
    void begin();
    bool checkGPS();
    double getLatitude();
    double getLongitude();

  private:
    uint8_t _rxPin, _txPin;
    uint32_t _baudRate;
    TinyGPSPlus _gps;
    SoftwareSerial *_gpsSerial;
};

#endif
