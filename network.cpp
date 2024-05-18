// network.cpp
#include "network.h"
#include <SoftwareSerial.h>
#include <TimerOne.h>

#define air780Serail  Serial
SoftwareSerial DebugSerial(10, 11); // RX, TX		

char phoneNumber[] = "13621822143";		 //替换成目标手机号
char TESTMSG[]     = "7EF453EF601D";	 //维可思	//中文短信，用uicode工具转换,然后把空格去掉	

unsigned long  Time_Cont = 0;         //定时器计数器

#define Success 1U
#define Failure 0U

int L = 13; //LED指示灯引脚

NetWork::NetWork(uint32_t baudRate)
:_baudRate(baudRate) {
}

void Timer1_handler(void)
{
	Time_Cont++;
}

// 函数：将单个字符转换为大写十六进制字符串
String charToHex(unsigned char c) {
  String hexString = String(c, HEX);
  hexString.toUpperCase(); // 转换为大写
  if (hexString.length() < 2) {
    hexString = "0" + hexString; // 确保每个字节用两位表示
  }
  return hexString;
}

// 函数：将字符串转换为Unicode编码并去掉空格
String convertToUnicode(const char* str) {
  String unicodeString = "";
  while (*str) {
    unsigned char c = (unsigned char)(*str);
    if (c < 0x80) {
      // 单字节字符（ASCII）
      unicodeString += charToHex(c);
    } else if (c >= 0x80) {
      // 多字节字符（UTF-8中的中文字符）
      unsigned int unicode = 0;
      if ((c & 0xF0) == 0xE0) {
        // 三字节UTF-8字符
        unicode = ((c & 0x0F) << 12) | (((unsigned char)(*(str + 1)) & 0x3F) << 6) | ((unsigned char)(*(str + 2)) & 0x3F);
        str += 2; // 跳过接下来的两个字节
      }
      // 将Unicode值转换为大写十六进制字符串
      String hexString = String(unicode, HEX);
      hexString.toUpperCase();
      while (hexString.length() < 4) {
        hexString = "0" + hexString; // 确保每个Unicode编码为4位
      }
      unicodeString += hexString;
    }
    str++;
  }
  return unicodeString;
}



void NetWork::begin() {
  DebugSerial.write("\r\n network begin!");
	DebugSerial.begin(_baudRate);
	air780Serail.begin(_baudRate);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(Timer1_handler);

	handAT();	//AT测试

	initair780();		                //初始化模块
  DebugSerial.write("\r\n network end!");
}

void NetWork::sendMessage(char *msg)
{
  String hexStr = convertToUnicode(msg);
  DebugSerial.println(hexStr);
	char send_buf[40] = {0};
	memset(send_buf, 0, 40);    //清空
	strcpy(send_buf, "AT+CMGS=\"");
	strcat(send_buf, phoneNumber);
	strcat(send_buf, "\"\r\n");
	if (sendCommand(send_buf, ">", 3000, 10) == Success);
	else errorLog(7);

	if (sendCommandHex(FIXMSG, 3000, 10) == Success);
	else errorLog(8);

	// memset(send_buf, 0, 40);    //清空
	// send_buf[0] = 0x1a;
	// send_buf[1] = '\0';
	// if (sendCommand(send_buf, "OK\r\n", 30000, 10) == Success);



	// air780Serail.write(0x1a);
	// delay(1000);

	// else errorLog(9);
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

	if (sendCommand("AT+CSMP=17,167,0,8\r\n", "OK\r\n", 3000, 10) == Success);
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
	DebugSerial.print("ERROR");
	DebugSerial.println(num);
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
			DebugSerial.print("\r\nRESET!!!!!!\r\n");
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

unsigned int NetWork::sendCommandHex(char *Command, unsigned long Timeout, unsigned char Retry)
{
	clrair780RxBuffer();
	for (unsigned char n = 0; n < Retry; n++)
	{
		char send_buf[100] = {0};
		memset(send_buf, 0, 100);    //清空
		DebugSerial.print("\r\n---------send AT Command:---------\r\n");
		DebugSerial.write(Command);
		
		unsigned char dat = 0,dat1 = 0,dat_hex = 0;
		DebugSerial.print("strlen=");
		DebugSerial.println(strlen(Command));
		for(int i = 0 ; i < strlen(Command) ;i++)
		{
			
			if(i%2==0)
			{
				dat = nibble2c(Command[i]);	//获取奇数位的数据
			}
				
			if(i%2==1)
			{
				dat1 = nibble2c(Command[i]);	//获取偶数位的数据

				dat_hex = hex2c(dat,dat1);	//汇总成一个HEX数据

				DebugSerial.print(dat_hex,HEX);	

				send_buf[i/2]=dat_hex;	//拼接到send_buf，等待发送
			}
					
		}

		DebugSerial.print("send_buf=");
		DebugSerial.print(send_buf);

		air780Serail.print(send_buf);

		// DebugSerial.print("\r\n---------out air780Serail.print(Command);---------\r\n");
		// DebugSerial.print(Time_Cont);
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			air780ReadBuffer();
			DebugSerial.print(".");	//1.8.x的IDE不加这个会有bug

			if (strstr(air780RxBuffer, send_buf) != NULL)
			{
				DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
				DebugSerial.print(air780RxBuffer); //输出接收到的信息
				clrair780RxBuffer();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
	DebugSerial.print(air780RxBuffer);//输出接收到的信息
	clrair780RxBuffer();
	return Failure;
}


unsigned int NetWork::sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	clrair780RxBuffer();
	for (unsigned char n = 0; n < Retry; n++)
	{
		DebugSerial.print("\r\n---------send AT Command:---------\r\n");
		DebugSerial.write(Command);

		air780Serail.write(Command);
		// DebugSerial.print("\r\n---------out air780Serail.print(Command);---------\r\n");
		// DebugSerial.print(Time_Cont);
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			air780ReadBuffer();
			DebugSerial.print(".");	//1.8.x的IDE不加这个会有bug

			if (strstr(air780RxBuffer, Response) != NULL)
			{
				DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
				DebugSerial.print(air780RxBuffer); //输出接收到的信息
				clrair780RxBuffer();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
	DebugSerial.print(air780RxBuffer);//输出接收到的信息
	clrair780RxBuffer();
	return Failure;
}

unsigned int NetWork::sendCommandReceive2Keyword(char *Command, char *Response, char *Response2, unsigned long Timeout, unsigned char Retry)
{
	clrair780RxBuffer();
	for (unsigned char n = 0; n < Retry; n++)
	{
		DebugSerial.print("\r\n---------send AT Command:---------\r\n");
		DebugSerial.write(Command);

		air780Serail.write(Command);

		Time_Cont = 0;
		
		while (Time_Cont < Timeout)
		{
			air780ReadBuffer();
			DebugSerial.print(".");	//1.8.x的IDE不加这个会有bug
			if (strstr(air780RxBuffer, Response) != NULL && strstr(air780RxBuffer, Response2) != NULL)
			{
				DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
				DebugSerial.print(air780RxBuffer); //输出接收到的信息
				clrair780RxBuffer();
				return Success;
			}
		}
		Time_Cont = 0;
	}
	DebugSerial.print("\r\n==========receive AT Command:==========\r\n");
	DebugSerial.print(air780RxBuffer);//输出接收到的信息
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

