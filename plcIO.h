#pragma once

#include <stdint.h>
#include "env.h"

#define INVALID_PIN ((uint8_t)-1)

enum IOTypeModel{IO_INVALID=0, IO_IN_DG_GEN=1, IO_IN_AL_GEN=101, IO_OUT_DG_GEN=201};

enum DevIDType{ID_TP_PIN, ID_TP_DOOR};

uint8_t inDgPins[QT_IN_DG]={33, 31, 29, 27, 25, 28};
uint8_t inAlPins[QT_IN_AL]={A0, A1, A2, A3, A4, A5};
uint8_t outDgPins[QT_OUT_DG]={32, 37, 39, 41, 43, 45};

class DeviceBase{
private:
  uint8_t pin;
  enum IOTypeModel tpMd;
protected:
  DeviceBase(uint8_t id, DevIDType idTp, IOTypeModel tpMd);
public:
  DeviceBase();

  uint8_t getPin();
  IOTypeModel getTypeModel();
};

template <enum IOTypeModel TM=IO_INVALID>
class Device:public DeviceBase{
public:
  Device();
  Device(uint8_t doorId);
};

template<>
class Device<IO_IN_DG_GEN>:public DeviceBase{
public:
  Device();
  Device(uint8_t doorId);
  uint8_t read();
};

template<>
class Device<IO_IN_AL_GEN>:public DeviceBase{
public:
  Device();
  Device(uint8_t doorId);
  int read();
};

template<>
class Device<IO_OUT_DG_GEN>:public DeviceBase{
public:
  Device();
  Device(uint8_t doorId);
  uint8_t read();
  void write(uint8_t val);
};