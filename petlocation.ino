#include "GPS.h"
#include "Buzz.h"
#include "network.h"
#include <SoftwareSerial.h>

// 定义引脚和波特率
const int GPSRXPin = 4;
const int GPSTXPin = 3;
const int GPSBaud = 9600;
const int BuzzPin = 13;

// 定义网络波特率和引脚
const int NetworkBaud = 9600;
SoftwareSerial DebugSerial(10, 11);     // RX, TX		

//调试参数
unsigned int debug_loop_times = 0;
const unsigned int debug_loop_times_max = 100;

// 创建对象
GPS gps(GPSRXPin, GPSTXPin, GPSBaud);
Buzz buzz(false, BuzzPin);
NetWork network(NetworkBaud, &DebugSerial);

void setup() {
  debug_loop_times = 0;
  Serial.begin(9600);
  DebugSerial.begin(9600);
  DebugSerial.println("setup is started");
  gps.begin();
  network.begin();
  DebugSerial.println("setup is ended");
}

void alermDis(int distance)
{
    // 示例字符串和整数值
    char send_buf[50] = {0};
		memset(send_buf, 0, 50);    //清空
    sprintf(send_buf, "your pet is leave home for %d m", distance);
    network.sendMessage(send_buf);
    DebugSerial.println(send_buf);
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

  return false;
}

void loop() {
  if (gps.checkGPS()) {
    double latitude  = gps.getLatitude();
    double longitude = gps.getLongitude();
    // 这里添加检查是否超出范围的逻辑
    if (isOutOfRange(latitude, longitude)) {
      // 示例字符串和整数值
    }
  }

  debug_loop_times = debug_loop_times +1;
  if(debug_loop_times == debug_loop_times_max){
    DebugSerial.print("alermDis is loop");
    DebugSerial.println(debug_loop_times);
    alermDis(100);
  }
  delay(100);
}
