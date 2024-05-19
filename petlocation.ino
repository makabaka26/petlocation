#include "GPS.h"
// #include "Buzz.h"
#include "network.h"
#include <SoftwareSerial.h>

// 默认波特率
const int DefaultBaud = 9600;

// 定义引脚和波特率
const uint8_t GPSRXPin = 5;
const uint8_t GPSTXPin = 6;

// 定义告警距离
const double AlarmDistance = 1000;
// 定义当前家的位置
const double HomeLat = 3102.20798;
const double HomeLon = 12128.11620;
// const int BuzzPin = 13;

// 告警的手机号
char PhoneNumber[] = "18018689132";		 //替换成目标手机号

// 定义网络波特率和引脚
SoftwareSerial DebugSerial(10, 11);     // RX, TX		

// 创建对象
GPS gps(GPSRXPin, GPSTXPin, DefaultBaud, &DebugSerial);
// Buzz buzz(false, BuzzPin);
NetWork network(DefaultBaud, &DebugSerial);

void setup() {
  Serial.begin(DefaultBaud);
  DebugSerial.begin(DefaultBaud);
  DebugSerial.println("setup is started");
  gps.begin();
  network.begin();
  DebugSerial.println("setup is ended");
}

void alermDis(int distance)
{
    // 示例字符串和整数值
    char send_buf[32] = {0};
		memset(send_buf, 0, 32);    //清空
    sprintf(send_buf, "your pet is leave home for %d m", distance);
    network.sendMessage(PhoneNumber, send_buf);
    DebugSerial.println(send_buf);
}

void loop() {
  if (gps.gpsRead()) {
    gps.parseGpsBuffer();
    gps.printGpsBuffer();
    // TODO 这边如何防止边界情况频繁进出， 如何在发送短信告警后，冷却一段时间
    int dis = gps.getDisFrom(HomeLat, HomeLon);
    if ( dis > AlarmDistance) {
      alermDis(dis);
    }
  }
}
