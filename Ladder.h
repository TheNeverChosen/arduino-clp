#pragma once

#include <stdint.h>
#include "CLPIO.h"

#define INVALID_VAL ((uint8_t)-1)

class LdVar{
private:
  uint16_t id;
public:
  LdVar();
  LdVar(uint16_t id);
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
  LdVarInternal(uint16_t id);
  LdVarInternal(uint16_t id, uint8_t startValue);

  uint8_t getValue();
  void setValue(uint8_t value);
};

template <enum IOTypeModel TM=IO_INVALID>
class LdVarDevice:public LdVar{
private:
  Device<TM> *device;
public:
  LdVarDevice();
  LdVarDevice(uint8_t id, DeviceBase *device);
  uint8_t getValue();
  void setValue(uint8_t value);
};

template <>
uint8_t LdVarDevice<IO_IN_DG_GEN>::getValue();

template <>
class LdVarDevice<IO_IN_AL_GEN>:public LdVar{
private:
  Device<IO_IN_AL_GEN> *device;
  int qtDivs, *divs;
  uint8_t *zoneVals, *dominances;
public:
  LdVarDevice();
  LdVarDevice(uint8_t id, DeviceBase *baseDev, int qtDivs, int *divs, uint8_t *zoneVals, uint8_t *dominances);
  ~LdVarDevice();
  uint8_t getValue();
};

template <>
uint8_t LdVarDevice<IO_OUT_DG>::getValue();
template <>
void LdVarDevice<IO_OUT_DG>::setValue(uint8_t value);

