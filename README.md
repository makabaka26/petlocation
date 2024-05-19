# petlocation

## 系统包含以下三个模块
1: gps  gps采集
2: network  4G网络模块，目前封装了发送英文短信
3: buzz   蜂鸣器 (未启用)
4: petlocation 宠物定位系统

## 本系统通过检测宠物位置，与家位置做比较，如果宠物离开家查过一定的范围, 触发短信告警

## 接线说明
1: 程序编译上传的适合，需要拔掉连接的0,1号串口
2: 将4G模块的Tx,RX 分别接入Arduino的RX, TX模块 (0, 1号串口), 4G模块的VIN, V_MCU, GND 分别接入Arduino的5V, 3.3V, GND
3: 将GPS模块的TX, RX 分别接入Arduino的RX, TX模块 (5, 6号串口), GPS模块的VCC, GND 分别接入Arduino的5V, GND
4: 如果需要调试，接入Arduino的RX, TX模块 (10, 11号串口)

## Lib库安装
## Copy下面两个库到这个目录 例如 C:\Users\lufei\Documents\Arduino\libraries
1: TinyGPSPlus
2: TimerOne

## 初始化完成说明
1: 4G模块插入SIM卡， 接入电源后，将PWK和GND连接1.5秒，直到4G模块出现闪灯
2: GPS的天线需要在室外或者靠近窗户, GPS模块出现闪灯
3：上述两个模块都出现闪灯，所有初始化全部完成

## 说明
1: 目前家庭初始化位置, 修改打开petlocation
const double HomeLat = 3102.20798;
const double HomeLon = 12128.11620;

2: 收告警的手机号
char PhoneNumber[] = "18018689132";	