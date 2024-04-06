#include "GPS.h"
#include "Buzz.h"

// 定义引脚和波特率
const int GPSRXPin = 4;
const int GPSTXPin = 3;
const int GPSBaud = 9600;
const int BuzzPin = 13;

// 创建对象
GPS gps(GPSRXPin, GPSTXPin, GPSBaud);
Buzz buzz(false, BuzzPin);

void setup() {
  Serial.begin(9600);
  gps.begin();
}

// 检查宠物是否超出预设距离的函数
bool isOutOfRange(double lat, double lon) {
  // // 这里应该有一些计算，比如与家的距离
  // // 为了示例，我们假设超出了1公里就是超出范围
  // // 这里需要替换成家的实际坐标
  // double homeLat = 34.0522; // 家的纬度
  // double homeLon = -118.2437; // 家的经度

  // // 计算当前位置与家的距离
  // double distance = TinyGPSPlus::distanceBetween(lat, lon, homeLat, homeLon);

  // return distance > 1000; // 距离超过10公里返回true

  // put your main code here, to run repeatedly:
  int val = Serial.read();
  if(val == 'R') {
    Serial.println("Hello world, My Name is Tony");
    return true;
  }
  return false;
}

void loop() {
  if (gps.checkGPS()) {
    double latitude = gps.getLatitude();
    double longitude = gps.getLongitude();
    // 这里添加检查是否超出范围的逻辑
    // 如果超出范围，激活蜂鸣器
    if (isOutOfRange(latitude, longitude)) {
      buzz.alarm();
    }
  }
  buzz.silence();
}
