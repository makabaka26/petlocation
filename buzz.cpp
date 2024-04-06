// Buzz.cpp
#include "Buzz.h"

Buzz::Buzz(bool high_buzz, uint8_t pin) : _pin(pin), _is_high_buzz(high_buzz) {
  pinMode(_pin, OUTPUT);
}

void Buzz::silence()
{
  if (_is_high_buzz){
    digitalWrite(_pin, LOW);
  }else{
    digitalWrite(_pin, HIGH);
  }
}

void Buzz::alarm() {
  if (_is_high_buzz){
    // 激活蜂鸣器
    digitalWrite(_pin, HIGH);
    delay(1000); // 持续1秒
    digitalWrite(_pin, LOW);
  }else{
    // 激活蜂鸣器
    digitalWrite(_pin, LOW);
    delay(1000); // 持续1秒
    digitalWrite(_pin, HIGH);
  }
}
