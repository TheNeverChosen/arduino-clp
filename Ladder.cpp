#include "CLPIO.h"
#include "Ladder.h"
#include <stdint.h>

LdVar::LdVar():id(-1){}
LdVar::LdVar(uint16_t id):id(id){}
uint16_t LdVar::getId(){
  return id;
}


LdVarInternal::LdVarInternal():LdVar(), value(LOW){}
LdVarInternal::LdVarInternal(uint16_t id):LdVar(id), value(LOW){}
LdVarInternal::LdVarInternal(uint16_t id, uint8_t startValue)
  :LdVar(id), value(startValue){}
uint8_t LdVarInternal::getValue(){
  return value;
}
void LdVarInternal::setValue(uint8_t value){
  this->value=value;
}


LdVarDevice::LdVarDevice():LdVar(), device(NULL){}
LdVarDevice::LdVarDevice(uint8_t id, Device *device)
  :LdVar(id), device(device){}
uint8_t LdVarDevice::getValue(){
  return device ? device->read() : -1;
}
void LdVarDevice::setValue(uint8_t value){
  if(device) device->write(value);
}