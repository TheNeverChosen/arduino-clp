#include <Arduino.h>
#include <stdint.h>
#include "CLPIO.h"
#include "env.h"

#define QT_IN_DG 6
#define QT_IN_AL 6
#define QT_OUT_DG 6

uint8_t inDgPins[QT_IN_DG]={27, 29, 31, 33, 35, 39};
uint8_t inAlPins[QT_IN_AL]={A0, A1, A2, A3, A4, A5};
uint8_t outDgPins[QT_OUT_DG]={41, 43, 45, 47, 53, 51};

DeviceBase::DeviceBase(uint8_t id, DevIDType idTp, IOTypeModel tpMd):tpMd(tpMd){
  this->pin = idTp==ID_TP_PIN
    ? id
    : tpMd==IO_IN_DG_GEN
      ? inDgPins[id]
      : tpMd==IO_IN_AL_GEN
        ? inAlPins[id]
        : tpMd==IO_OUT_DG
        ? outDgPins[id]
        : IO_INVALID;
}
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
Device<TM>::Device(uint8_t doorId):DeviceBase(doorId, ID_TP_DOOR,TM){}


Device<IO_IN_DG_GEN>::Device():DeviceBase(){}
Device<IO_IN_DG_GEN>::Device(uint8_t doorId):DeviceBase(doorId, ID_TP_DOOR, IO_IN_DG_GEN){
  pinMode(getPin(), INPUT);
}
uint8_t Device<IO_IN_DG_GEN>::read(){
  uint8_t val = digitalRead(getPin());
  return val;
}

Device<IO_IN_AL_GEN>::Device():DeviceBase(){}
Device<IO_IN_AL_GEN>::Device(uint8_t doorId):DeviceBase(doorId, ID_TP_DOOR, IO_IN_AL_GEN){
}
int Device<IO_IN_AL_GEN>::read(){
  int val = analogRead(getPin());
  // /////////////////////debugagem/////////////////////
  // Serial.print("Pino: "); Serial.println(getPin());
  // Serial.print(" Valor lido: "); Serial.println(val);
  // /////////////////////debugagem/////////////////////
  return val;
}


Device<IO_OUT_DG>::Device():DeviceBase(){}
Device<IO_OUT_DG>::Device(uint8_t doorId):DeviceBase(doorId, ID_TP_DOOR, IO_OUT_DG){
  pinMode(getPin(), OUTPUT);
}
uint8_t Device<IO_OUT_DG>::read(){
  return digitalRead(getPin());
}
void Device<IO_OUT_DG>::write(uint8_t val){
  digitalWrite(getPin(), val);
}