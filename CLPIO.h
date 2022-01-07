#pragma once

#include <stdint.h>

#define INVALID_PIN ((uint8_t)-1)

enum IOTypeModel{IO_INVALID=0, IO_IN_DG_GEN=1, IO_IN_AL_GEN=101, IO_OUT_DG_GEN=201};

enum DevIDType{ID_TP_PIN, ID_TP_DOOR};

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