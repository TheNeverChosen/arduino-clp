#include <stdint.h>
#include "CLPIO.h"

#define QT_IN_DG 4
#define QT_IN_AL 4
#define QT_OUT_DG 4

#define asdad 5

uint8_t inDgPins[QT_IN_DG]={22, 23, 24, 25};
uint8_t inAlPins[QT_IN_AL]={A0, A1, A2, A3};
uint8_t outDgPins[QT_OUT_DG]={26, 27, 28, 35};

BaseDevice::BaseDevice(uint8_t pin, IOTypeModel tpMd):pin(pin), tpMd(tpMd){}
BaseDevice::BaseDevice():pin(INVALID_PIN), tpMd(IO_INVALID){}
uint8_t BaseDevice::getPin(){
  return pin;
}
IOTypeModel BaseDevice::getTypeModel(){
  return tpMd;
}


template <enum IOTypeModel TM>
Device<TM>::Device():BaseDevice(){}
template <enum IOTypeModel TM>
Device<TM>::Device(uint8_t pin):BaseDevice(pin, TM){}


Device<IO_IN_DG_GEN>::Device():BaseDevice(){}
Device<IO_IN_DG_GEN>::Device(uint8_t pin):BaseDevice(pin, IO_IN_DG_GEN){
  pinMode(pin, INPUT);
}
uint8_t Device<IO_IN_DG_GEN>::read(){
  return digitalRead(getPin());
}


Device<IO_IN_AL_GEN>::Device():BaseDevice(){}
Device<IO_IN_AL_GEN>::Device(uint8_t pin):BaseDevice(pin, IO_IN_AL_GEN){
  pinMode(pin, INPUT);
}
int Device<IO_IN_AL_GEN>::read(){
  return analogRead(getPin());
}


Device<IO_OUT_DG>::Device():BaseDevice(){}
Device<IO_OUT_DG>::Device(uint8_t pin):BaseDevice(pin, IO_OUT_DG){
  pinMode(pin, OUTPUT);
}
uint8_t Device<IO_OUT_DG>::read(){
  return digitalRead(getPin());
}
void Device<IO_OUT_DG>::write(uint8_t val){
  digitalWrite(getPin(), val);
}