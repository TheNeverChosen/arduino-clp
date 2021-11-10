#include <stdint.h>
#include "CLPIO.h"

#define QT_IN_DG 6
#define QT_IN_AL 6
#define QT_OUT_DG 6

uint8_t inDgPins[QT_IN_DG]={22, 23, 24, 25, 26, 27};
uint8_t inAlPins[QT_IN_AL]={A0, A1, A2, A3, A4, A5};
uint8_t outDgPins[QT_OUT_DG]={28, 29, 30, 31, 32, 33};

DeviceBase::DeviceBase(uint8_t pin, IOTypeModel tpMd):pin(pin), tpMd(tpMd){}
DeviceBase::DeviceBase():pin(INVALID_PIN), tpMd(IO_INVALID){}
uint8_t DeviceBase::getPin(){
  return pin;
}
IOTypeModel DeviceBase::getTypeModel(){
  return tpMd;
}


template <enum IOTypeModel TM>
Device<TM>::Device():DeviceBase(){}
template <enum IOTypeModel TM>
Device<TM>::Device(uint8_t pin):DeviceBase(pin, TM){}


Device<IO_IN_DG_GEN>::Device():DeviceBase(){}
Device<IO_IN_DG_GEN>::Device(uint8_t pin):DeviceBase(pin, IO_IN_DG_GEN){
  pinMode(pin, INPUT);
}
uint8_t Device<IO_IN_DG_GEN>::read(){
  return digitalRead(getPin());
}


Device<IO_IN_AL_GEN>::Device():DeviceBase(){}
Device<IO_IN_AL_GEN>::Device(uint8_t pin):DeviceBase(pin, IO_IN_AL_GEN){
  pinMode(pin, INPUT);
}
int Device<IO_IN_AL_GEN>::read(){
  return analogRead(getPin());
}


Device<IO_OUT_DG>::Device():DeviceBase(){}
Device<IO_OUT_DG>::Device(uint8_t pin):DeviceBase(pin, IO_OUT_DG){
  pinMode(pin, OUTPUT);
}
uint8_t Device<IO_OUT_DG>::read(){
  return digitalRead(getPin());
}
void Device<IO_OUT_DG>::write(uint8_t val){
  digitalWrite(getPin(), val);
}