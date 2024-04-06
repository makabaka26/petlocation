// GPS.cpp
#include "GPS.h"
#include <SoftwareSerial.h>

GPS::GPS(uint8_t rxPin, uint8_t txPin, uint32_t baudRate)
: _rxPin(rxPin), _txPin(txPin), _baudRate(baudRate) {
  _gpsSerial = new SoftwareSerial(_rxPin, _txPin);
}

void GPS::begin() {
  _gpsSerial->begin(_baudRate);
}

bool GPS::checkGPS() {
  // while (_gpsSerial->available() > 0) {
  //   if (_gps.encode(_gpsSerial->read())) {
  //     return true;
  //   }
  // }
  // return false;
  return true;
}

double GPS::getLatitude() {
  return _gps.location.lat();
}

double GPS::getLongitude() {
  return _gps.location.lng();
}
