// network.h
#ifndef NETWORK_h
#define NETWORK_h

#include <Arduino.h>

const unsigned int AirRxBufferLength = 600;

class SoftwareSerial;
class NetWork {
  public:
    NetWork(uint32_t baudRate, SoftwareSerial *debugSerial);
    void begin();
    void sendMessage(char *msg);
  private:
    void initair780();
    void handAT();
    void errorLog(int num);
    unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
    unsigned int sendCommandReceive2Keyword(char *Command, char *Response, char *Response2, unsigned long Timeout, unsigned char Retry);

    void air780ReadBuffer();
    void clrair780RxBuffer(void);
  private:
    uint32_t _baudRate;                 //网络波特率
    SoftwareSerial *debugSerial;

    char air780RxBuffer[AirRxBufferLength];   //发送缓冲区
    unsigned int air780BufferCount = 0;		
};

#endif