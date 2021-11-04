#include <stdint.h>
#include "CLPIO.h"

#define QT_IN_DG 4
#define QT_IN_AL 4
#define QT_OUT_DG 4

uint8_t inDgPins[QT_IN_DG]={22, 23, 24, 25};
uint8_t inAlPins[QT_IN_AL]={A0, A1, A2, A3};
uint8_t outDgPins[QT_OUT_DG]={26, 27, 28, 35};

Device::Device():pin(0), tpMd(IO_INVALID){}
Device::Device(uint8_t pin, IOTypeModel tpMd):pin(pin), tpMd(tpMd){}
uint8_t Device::getPin(){
  return pin;
}
IOTypeModel Device::getTypeModel(){
  return tpMd;
}


InDgGen::InDgGen():Device(){}
InDgGen::InDgGen(uint8_t pin):Device(pin, IO_IN_DG_GEN){
  pinMode(pin, INPUT);
}
uint8_t InDgGen::read(void *data=NULL){
  return digitalRead(getPin());
}
void InDgGen::write(uint8_t val){
  Serial.println("(InDgGen) INPUT write: Invalid.");
}


OutDg::OutDg():Device(){}
OutDg::OutDg(uint8_t pin):Device(pin, IO_OUT_DG){
  pinMode(pin, OUTPUT);
}
uint8_t OutDg::read(){
  return digitalRead(getPin());
}
void OutDg::write(uint8_t val){
  digitalWrite(getPin(), val);
}
