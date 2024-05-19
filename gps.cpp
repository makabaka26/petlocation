/*********************************************************************
功能说明：通过解析GPS帧数据，获取需要的帧数据GPRMC或者GNRMC，
          从中解析出来UTC时间和经纬度，并通过串口打印输出。
开发环境：代码在Arduino UNO上，开发环境使用Arduino IDE 1.0.6版本测试		  
接线方式：		  
请先下载程序然后再接线，否则接上GPS后会形象UNO板子下载程序！！！
GPS模块VCC--------------Arduino的5v 
GPS模块GND--------------Arduino的GND 
GPS模块TXD--------------Arduino的D0（数字IO  0） 
GPS模块RXD不接
GPS模块PPS不接
特别提醒：其中GPS的天线记得要接，并且要室外放置，因为GPS模块是室外定位的，
          室内没有卫星信号。首次定位需要下载卫星数据，定时时间从1-10分钟
	  不等，看具体的所处的环境。
补充说明：本代码仅适用于UNO,NANO,PRO MINI， 
          若使用leonardo或者mega2560等多串口板子请参考其它Serial定义，
	  需要修改定义代码和接线。		  
*********************************************************************/
//此处为了兼容其他的多串口Arduino板子
#include "gps.h"
#include <TinyGPS++.h>

uint8_t LedPin = 13; //LED指示灯引脚

struct
{
	char GPS_Buffer[80];
	bool isGetData;		//是否获取到GPS数据
	bool isParseData;	//是否解析完成
	char UTCTime[11];	//UTC时间
	char latitude[11];	//纬度
	char N_S[2];		//N/S
	char longitude[12];	//经度
	char E_W[2];		//E/W
	bool isUsefull;		//定位信息是否有效
} Save_Data;

const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;

GPS::GPS(uint8_t rxPin, uint8_t txPin, uint32_t baudRate, SoftwareSerial *_debugSerial)
:  _baudRate(baudRate), debugSerial(_debugSerial), gpsSerial(rxPin, txPin) {
}

void  GPS::begin()	//初始化内容
{
	gpsSerial.begin(9600);			
	debugSerial->println("Welcome to use");
	debugSerial->println("Wating...");

	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
}

void  GPS::errorLog(int num)
{
	debugSerial->print("ERROR");
	debugSerial->println(num);
	while (1)
	{
		digitalWrite(LedPin, HIGH);
		delay(300);
		digitalWrite(LedPin, LOW);
		delay(300);
	}
}

void GPS::printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		
		debugSerial->print("Save_Data.UTCTime = ");
		debugSerial->println(Save_Data.UTCTime);

		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			debugSerial->print("Save_Data.latitude = ");
			debugSerial->println(Save_Data.latitude);
			debugSerial->print("Save_Data.N_S = ");
			debugSerial->println(Save_Data.N_S);
			debugSerial->print("Save_Data.longitude = ");
			debugSerial->println(Save_Data.longitude);
			debugSerial->print("Save_Data.E_W = ");
			debugSerial->println(Save_Data.E_W);
		}
		else
		{
			debugSerial->println("GPS DATA is not usefull!");
		}		
	}
}

void  GPS::parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		debugSerial->println("**************");
		debugSerial->println(Save_Data.GPS_Buffer);

		
		for (int i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}
		}
	}
}


bool  GPS::gpsRead() 
{
	while (gpsSerial.available())
	{
		gpsRxBuffer[ii++] = gpsSerial.read();
		if (ii == gpsRxBufferLength)clrGpsRxBuffer();
	}

	char* GPS_BufferHead;
	char* GPS_BufferTail;
	if ((GPS_BufferHead = strstr(gpsRxBuffer, "$GPRMC,")) != NULL || (GPS_BufferHead = strstr(gpsRxBuffer, "$GNRMC,")) != NULL )
	{
		if (((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL) && (GPS_BufferTail > GPS_BufferHead))
		{
			memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);
			Save_Data.isGetData = true;

			clrGpsRxBuffer();
      return true;
		}
	}
  return false;
}

void  GPS::clrGpsRxBuffer(void)
{
	memset(gpsRxBuffer, 0, gpsRxBufferLength);      //清空
	ii = 0;
}

double  GPS::getDisFrom(double baseLat, double baseLon)
{
  if(Save_Data.isUsefull){
		double latitude  = atof(Save_Data.latitude);
		double longitude = atof(Save_Data.longitude);
		return TinyGPSPlus::distanceBetween(latitude, longitude, baseLat, baseLon);
  }
  return -1;
}
