#pragma once

#include <stdint.h>
#include "plcIO.h"
#include "env.h"

#define INVALID_VAL ((uint8_t)-1)

class LdVar{
private:
  sz_varr id;
public:
  LdVar();
  LdVar(sz_varr id);
  virtual ~LdVar();

  uint16_t getId();

  virtual uint8_t getValue();
  virtual void setValue(uint8_t value);
};

class LdVarInternal:public LdVar{
private:
  uint8_t value;
public:
  LdVarInternal();
  LdVarInternal(sz_varr id);
  LdVarInternal(sz_varr id, uint8_t startValue);

  uint8_t getValue();
  void setValue(uint8_t value);
};

//====================LdVarDevice<TM>====================
template <enum IOTypeModel TM=IO_INVALID>
class LdVarDevice:public LdVar{
private:
  Device<TM> *device;
public:
  LdVarDevice();
  LdVarDevice(sz_varr id, DeviceBase *device);
  uint8_t getValue();
  void setValue(uint8_t value);
};

template <enum IOTypeModel TM>
LdVarDevice<TM>::LdVarDevice():LdVar(),device(nullptr){}

template <enum IOTypeModel TM>
LdVarDevice<TM>::LdVarDevice(sz_varr id, DeviceBase *baseDev)
  :LdVar(id), device(static_cast<Device<TM>*>(baseDev)){}

template <enum IOTypeModel TM>
uint8_t LdVarDevice<TM>::getValue(){
  return LdVar::getValue();
}

template <enum IOTypeModel TM>
void LdVarDevice<TM>::setValue(uint8_t value){
  LdVar::setValue(value);
}
//=======================================================

template <>
uint8_t LdVarDevice<IO_IN_DG_GEN>::getValue();

template <>
class LdVarDevice<IO_IN_AL_GEN>:public LdVar{
private:
  Device<IO_IN_AL_GEN> *device;
  uint16_t qtDivs, *divs;
  uint8_t *zoneVals, *dominances;
public:
  LdVarDevice();
  LdVarDevice(sz_varr id, DeviceBase *baseDev, uint16_t qtDivs, uint16_t *divs, uint8_t *zoneVals, uint8_t *dominances);
  ~LdVarDevice();
  uint8_t getValue();
};

template <>
uint8_t LdVarDevice<IO_OUT_DG_GEN>::getValue();
template <>
void LdVarDevice<IO_OUT_DG_GEN>::setValue(uint8_t value);