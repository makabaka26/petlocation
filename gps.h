// GPS.h
#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

#include <SoftwareSerial.h>

class SoftwareSerial;

class GPS {
  public:
    GPS(uint8_t rxPin, uint8_t txPin, uint32_t baudRate, SoftwareSerial *_debugSerial);
    void begin();

    //gps 位置获取
    bool gpsRead();
    //解析GPS位置
    void parseGpsBuffer();
    //返回当前GPS位置与base的距离
    double  getDisFrom(double baseLat, double baseLon);
    //输入GPS 位置信息
    void printGpsBuffer();
  private:
    void  errorLog(int num);
    void  clrGpsRxBuffer(void);
  private:
    uint32_t _baudRate;
    SoftwareSerial *debugSerial;
    SoftwareSerial gpsSerial;
};


#endif
