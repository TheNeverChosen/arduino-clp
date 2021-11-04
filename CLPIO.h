#pragma once

#include <stdint.h>

enum IOTypeModel{IO_INVALID, IO_INTERNAL, IO_IN_DG_GEN, IN_AL_GEN, IO_OUT_DG};

class Device{
private:
  uint8_t pin;
  enum IOTypeModel tpMd;
public:
  Device();
  Device(uint8_t pin, IOTypeModel tpMd);

  uint8_t getPin();
  IOTypeModel getTypeModel();

  virtual uint8_t read() = 0;
  virtual void write(uint8_t val) = 0;
};

class InDgGen:public Device{
public:
  InDgGen();
  InDgGen(uint8_t pin);

  uint8_t read();
  void write(uint8_t val);
};

class OutDg:public Device{
public:
  OutDg();
  OutDg(uint8_t pin);

  uint8_t read();
  void write(uint8_t val);
};

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