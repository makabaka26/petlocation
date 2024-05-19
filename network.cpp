// network.cpp
#include "network.h"
#include <TimerOne.h>
#include <SoftwareSerial.h>

#define air780Serail  Serial

char phoneNumber[] = "18018689132";		 //替换成目标手机号
char TESTMSG[]     = "7EF453EF601D";	 //维可思	//中文短信，用uicode工具转换,然后把空格去掉	

unsigned long  Time_Cont = 0;         //定时器计数器

#define Success 1U
#define Failure 0U

int L = 13; //LED指示灯引脚

NetWork::NetWork(uint32_t baudRate, SoftwareSerial *_debugSerial)
:_baudRate(baudRate),
  debugSerial(_debugSerial)
 {
}

void Timer1_handler(void)
{
	Time_Cont++;
}

void NetWork::begin() {
  debugSerial->write("\r\n network begin!");
	air780Serail.begin(_baudRate);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(Timer1_handler);

	handAT();	//AT测试

	initair780();		                //初始化模块
  debugSerial->write("\r\n network end!");
}

void NetWork::sendMessage(char *msg)
{
	char send_buf[40] = {0};
	memset(send_buf, 0, 40);    //清空
	strcpy(send_buf, "AT+CMGS=\"");
	strcat(send_buf, phoneNumber);
	strcat(send_buf, "\"\r\n");
	if (sendCommand(send_buf, ">", 3000, 10) == Success);
	else errorLog(7);

	if (sendCommand(msg, msg, 3000, 10) == Success);
	else errorLog(8);

	memset(send_buf, 0, 40);    //清空
	send_buf[0] = 0x1a;
	send_buf[1] = '\0';
	if (sendCommand(send_buf, "OK\r\n", 30000, 10) == Success);
}

void NetWork::initair780()
{
	if (sendCommand("AT\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);

	if (sendCommand("AT&F\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);

	if (sendCommand("AT+CSQ\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	delay(100);

	if (sendCommandReceive2Keyword("AT+CPIN?\r\n", "READY","OK\r\n" ,3000, 10) == Success);
	else errorLog(1);
	delay(100);

	if (sendCommand("AT+COPS?\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	delay(100);

	if (sendCommandReceive2Keyword("AT+CREG?\r\n", ",1","OK\r\n", 3000, 10) == Success);	//本地SIM卡
	else if(sendCommandReceive2Keyword("AT+CREG?\r\n", ",5", "OK\r\n",3000, 10) == Success	);//漫游SIM卡
	else errorLog(3);

	if (sendCommand("AT+CMGF=1\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(5);
	delay(10);

	if (sendCommand("AT+CSCS?\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(5);
	delay(10);

	if (sendCommand("AT+CSMP=17,167,0,0\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(6);
	delay(10);
}

void(* resetFunc) (void) = 0; //制造重启命令

void NetWork::handAT()
{
	while (sendCommand("AT\r\n", "OK\r\n", 100, 10) != Success)
	{
		delay(100);
	}	
}

void NetWork::errorLog(int num)
{
	debugSerial->print("ERROR");
	debugSerial->println(num);
	while (1)
	{
		digitalWrite(L, HIGH);
		delay(100);
		digitalWrite(L, LOW);
		delay(100);
		digitalWrite(L, HIGH);
		delay(100);
		digitalWrite(L, LOW);
		delay(300);

		if (sendCommand("AT\r\n", "OK\r\n", 100, 10) == Success)
		{
			debugSerial->print("\r\nRESET!!!!!!\r\n");
			resetFunc();
		}
	}
}

unsigned char nibble2c(unsigned char c)
{
   if ((c>='0') && (c<='9'))
      return c-'0' ;
   if ((c>='A') && (c<='F'))
      return c+10-'A' ;
   if ((c>='a') && (c<='a'))
      return c+10-'a' ;
   return -1 ;
}

unsigned char hex2c(unsigned char c1, unsigned char c2)
{
   if(c1 >= 0)
      return c1*16 + c2 ;
   return 0 ;
}

unsigned int NetWork::sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	clrair780RxBuffer();
	for (unsigned char n = 0; n < Retry; n++)
	{
		debugSerial->print("\r\n---------send AT Command:---------\r\n");
		debugSerial->write(Command);

		air780Serail.write(Command);
		// debugSerial->print("\r\n---------out air780Serail.print(Command);---------\r\n");
		// debugSerial->print(Time_Cont);
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			air780ReadBuffer();
			debugSerial->print(".");	//1.8.x的IDE不加这个会有bug

			if (strstr(air780RxBuffer, Response) != NULL)
			{
				debugSerial->print("\r\n==========receive AT Command:==========\r\n");
				debugSerial->print(air780RxBuffer); //输出接收到的信息
				clrair780RxBuffer();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	debugSerial->print("\r\n==========receive AT Command:==========\r\n");
	debugSerial->print(air780RxBuffer);//输出接收到的信息
	clrair780RxBuffer();
	return Failure;
}

unsigned int NetWork::sendCommandReceive2Keyword(char *Command, char *Response, char *Response2, unsigned long Timeout, unsigned char Retry)
{
	clrair780RxBuffer();
	for (unsigned char n = 0; n < Retry; n++)
	{
		debugSerial->print("\r\n---------send AT Command:---------\r\n");
		debugSerial->write(Command);

		air780Serail.write(Command);

		Time_Cont = 0;
		
		while (Time_Cont < Timeout)
		{
			air780ReadBuffer();
			debugSerial->print(".");	//1.8.x的IDE不加这个会有bug
			if (strstr(air780RxBuffer, Response) != NULL && strstr(air780RxBuffer, Response2) != NULL)
			{
				debugSerial->print("\r\n==========receive AT Command:==========\r\n");
				debugSerial->print(air780RxBuffer); //输出接收到的信息
				clrair780RxBuffer();
				return Success;
			}
		}
		Time_Cont = 0;
	}
	debugSerial->print("\r\n==========receive AT Command:==========\r\n");
	debugSerial->print(air780RxBuffer);//输出接收到的信息
	clrair780RxBuffer();
	return Failure;
}



void NetWork::air780ReadBuffer() 
{
	while (air780Serail.available())
	{
		air780RxBuffer[air780BufferCount++] = air780Serail.read();
		if (air780BufferCount == AirRxBufferLength)clrair780RxBuffer();
	}
}

void NetWork::clrair780RxBuffer(void)
{
	memset(air780RxBuffer, 0, AirRxBufferLength); //清空
	air780BufferCount = 0;
}

