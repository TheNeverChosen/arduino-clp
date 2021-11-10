#include <Arduino.h>
#include <stdint.h>
#include "CLPIO.h"
#include "Ladder.h"
#include "utils.h"

#define DEF_INTERNAL_VAL LOW //Default Value for LdVarInternal

//=========================Utils=========================
uint8_t getValueUnsupported(uint8_t id){
  Serial.print("getValue not supported in VarId ");
  Serial.println(id);
  return INVALID_VAL;
}

void setValueUnsupported(uint8_t id){
  Serial.print("setValue not supported in VarId ");
  Serial.println(id);
}
//=======================================================

//=========================LdVar=========================
LdVar::LdVar():id(-1){}

LdVar::LdVar(uint16_t id):id(id){}

uint16_t LdVar::getId(){
  return id;
}
//=======================================================

//=====================LdVarInternal=====================
LdVarInternal::LdVarInternal():LdVar(), value(DEF_INTERNAL_VAL){}

LdVarInternal::LdVarInternal(uint16_t id):LdVar(id), value(LOW){}

LdVarInternal::LdVarInternal(uint16_t id, uint8_t startValue)
  :LdVar(id), value(startValue){}
uint8_t LdVarInternal::getValue(){
  return value;
}

void LdVarInternal::setValue(uint8_t value){
  this->value=value;
}
//=======================================================

//====================LdVarDevice<TM>====================
template <enum IOTypeModel TM>
LdVarDevice<TM>::LdVarDevice():LdVar(),device(nullptr){}

template <enum IOTypeModel TM>
LdVarDevice<TM>::LdVarDevice(uint8_t id, DeviceBase *baseDev)
  :LdVar(id), device(static_cast<Device<TM>*>(baseDev)){}

template <enum IOTypeModel TM>
uint8_t LdVarDevice<TM>::getValue(){
  return getValueUnsupported(getId());
}

template <enum IOTypeModel TM>
void LdVarDevice<TM>::setValue(uint8_t value){
  setValueUnsupported(getId());
}
//=======================================================

//===============LdVarDevice<IO_IN_DG_GEN>===============
template<>
uint8_t LdVarDevice<IO_IN_DG_GEN>::getValue(){
  if(device==nullptr) return getValueUnsupported(getId());
  return device->read();
}
//=======================================================

//===============LdVarDevice<IO_IN_AL_GEN>===============
LdVarDevice<IO_IN_AL_GEN>::LdVarDevice()
  :LdVar(),device(nullptr),qtDivs(0),divs(nullptr)
  ,zoneVals(nullptr),dominances(nullptr){}

LdVarDevice<IO_IN_AL_GEN>::LdVarDevice(uint8_t id, DeviceBase *baseDev,
  int qtDivs, int *divs, uint8_t *zoneVals, uint8_t *dominances)
    :LdVar(id),device(static_cast<Device<IO_IN_AL_GEN>*>(baseDev)),
    qtDivs(qtDivs),divs(divs),zoneVals(zoneVals),dominances(dominances){
  if(qtDivs<0 || zoneVals==nullptr ||
    (qtDivs>0 && (divs==nullptr || dominances==nullptr)))
    device=nullptr; //invalid
}

LdVarDevice<IO_IN_AL_GEN>::~LdVarDevice(){
  delete[] divs;
  delete[] zoneVals;
  delete[] dominances;
}

uint8_t LdVarDevice<IO_IN_AL_GEN>::getValue(){
  if(device==nullptr) return getValueUnsupported(getId());
  int k=device->read(),
      sg = smallestGreater<int>(divs, qtDivs, k);

  return (sg==qtDivs || divs[sg]!=k || dominances[sg]==0)
    ? zoneVals[sg]
    : zoneVals[sg+1];
}
//=======================================================

//================LdVarDevice<IO_OUT_DG>=================
template<>
uint8_t LdVarDevice<IO_OUT_DG>::getValue(){
  if(device==nullptr) return getValueUnsupported(getId());
  return device->read();
}

template<>
void LdVarDevice<IO_OUT_DG>::setValue(uint8_t value){
  if(device==nullptr) setValueUnsupported(getId());
  else device->write(value);
}
//=======================================================


