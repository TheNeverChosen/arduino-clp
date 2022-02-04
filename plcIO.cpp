#include <Arduino.h>
#include <stdint.h>
#include "plcIO.h"
#include "env.h"

DeviceBase::DeviceBase(uint8_t id, DevIDType idTp, IOTypeModel tpMd):tpMd(tpMd){
  this->pin = idTp==ID_TP_PIN
    ? id
    : tpMd==IO_IN_DG_GEN
      ? inDgPins[id]
      : tpMd==IO_IN_AL_GEN
        ? inAlPins[id]
        : tpMd==IO_OUT_DG_GEN
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
  #ifdef DEBUG_ON 
    Serial.print(F("Pino: ")); Serial.println(getPin());
    Serial.print(F(" Valor lido: ")); Serial.println(val);
  #endif
  return val;
}


Device<IO_OUT_DG_GEN>::Device():DeviceBase(){}
Device<IO_OUT_DG_GEN>::Device(uint8_t doorId):DeviceBase(doorId, ID_TP_DOOR, IO_OUT_DG_GEN){
  pinMode(getPin(), OUTPUT);
}
uint8_t Device<IO_OUT_DG_GEN>::read(){
  return digitalRead(getPin());
}
void Device<IO_OUT_DG_GEN>::write(uint8_t val){
  digitalWrite(getPin(), val);
}