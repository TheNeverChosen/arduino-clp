#pragma once

#include "CLPIO.h"

class LdVar{
private:
  uint16_t id;
public:
  LdVar();
  LdVar(uint16_t id);

  uint16_t getId();

  virtual uint8_t getValue() = 0;
  virtual void setValue(uint8_t value) = 0;
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

class LdVarDevice:public LdVar{
private:
  Device *device;
public:
  LdVarDevice();
  LdVarDevice(uint8_t id, Device *device);
  
  uint8_t getValue();
  void setValue(uint8_t value);
};