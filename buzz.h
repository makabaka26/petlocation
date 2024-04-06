// Buzz.h
#ifndef Buzz_h
#define Buzz_h

#include <Arduino.h>

class Buzz {
  public:
    Buzz(bool high_buzz, uint8_t pin);
    void alarm();
    void silence();

  private:
    uint8_t _pin;
    bool    _is_high_buzz;
};

#endif