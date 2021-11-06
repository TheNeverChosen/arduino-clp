#pragma once

#include <stdint.h>

#define INVALID_PIN ((uint8_t)-1)
#define INVALID_VAL ((uint8_t)-1)

enum IOTypeModel{IO_INVALID=0, IO_IN_DG_GEN=1, IO_IN_AL_GEN=101, IO_OUT_DG=201};

class BaseDevice{
private:
  uint8_t pin;
  enum IOTypeModel tpMd;
protected:
  BaseDevice(uint8_t pin, IOTypeModel tpMd);
public:
  BaseDevice();
  virtual ~BaseDevice();

  uint8_t getPin();
  IOTypeModel getTypeModel();
};

template <enum IOTypeModel TM=IO_INVALID>
class Device:public BaseDevice{
public:
  Device();
  Device(uint8_t pin);
};

template<>
class Device<IO_IN_DG_GEN>:public BaseDevice{
public:
  Device();
  Device(uint8_t pin);
  uint8_t read();
};

template<>
class Device<IO_IN_AL_GEN>:public BaseDevice{
public:
  Device();
  Device(uint8_t pin);
  int read();
};

template<>
class Device<IO_OUT_DG>:public BaseDevice{
public:
  Device();
  Device(uint8_t pin);
  uint8_t read();
  void write(uint8_t val);
};