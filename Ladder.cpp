#include <Arduino.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include "plcIO.h"
#include "Ladder.h"
#include "utils.h"
#include "env.h"

#define DEF_INTERNAL_VAL LOW //Default Value for LdVarInternal

//=========================LdVar=========================
LdVar::LdVar():id(-1){}

LdVar::LdVar(sz_varr id):id(id){}

LdVar::~LdVar(){}

uint16_t LdVar::getId(){
  return id;
}

uint8_t LdVar::getValue(){
  #ifdef DEBUG_ON 
   Serial.print("getValue not supported in VarId ");
   Serial.println(id); 
  #endif
  return INVALID_VAL;
}

void LdVar::setValue(uint8_t value){
  Serial.print(F("setValue not supported in VarId ")); 
  Serial.println(id);
}
//=======================================================

//=====================LdVarInternal=====================
LdVarInternal::LdVarInternal():LdVar(), value(DEF_INTERNAL_VAL){}

LdVarInternal::LdVarInternal(sz_varr id):LdVar(id), value(LOW){}

LdVarInternal::LdVarInternal(sz_varr id, uint8_t startValue)
  :LdVar(id), value(startValue){}

uint8_t LdVarInternal::getValue(){
  #ifdef DEBUG_ON
    Serial.print(F("Valor interno lido: ")); Serial.println(value);
  #endif
  return value;
}

void LdVarInternal::setValue(uint8_t value){
  this->value=value;
  #ifdef DEBUG_ON
    Serial.print(F("Valor interno setado: ")); Serial.println(value);
  #endif
}
//=======================================================

//===============LdVarDevice<IO_IN_DG_GEN>===============
template<>
uint8_t LdVarDevice<IO_IN_DG_GEN>::getValue(){
  if(device==nullptr) return LdVar::getValue();

  uint8_t val = device->read();

  #ifdef DEBUG_ON
    Serial.print(F("Pino: ")); Serial.print(device->getPin());
    Serial.print(F(" // Valor lido: ")); Serial.println(val);
  #endif

  return val;
}
//=======================================================

//===============LdVarDevice<IO_IN_AL_GEN>===============
LdVarDevice<IO_IN_AL_GEN>::LdVarDevice()
  :LdVar(),device(nullptr),qtDivs(0),divs(nullptr)
  ,zoneVals(nullptr),dominances(nullptr){}

LdVarDevice<IO_IN_AL_GEN>::LdVarDevice(sz_varr id, DeviceBase *baseDev,
  uint16_t qtDivs, uint16_t *divs, uint8_t *zoneVals, uint8_t *dominances)
    :LdVar(id),device(static_cast<Device<IO_IN_AL_GEN>*>(baseDev)),
    qtDivs(qtDivs),divs(divs),zoneVals(zoneVals),dominances(dominances){
  if(qtDivs<0 || zoneVals==nullptr ||
    (qtDivs>0 && (divs==nullptr || dominances==nullptr)))
    device=nullptr; //invalid
  #ifdef DEBUG_ON 
    Serial.println(F("==========DIVS==========")); 
  #endif
  for(int i=0;i<qtDivs;i++)
    Serial.println(divs[i]);

  uint16_t zoneValsSz = ((qtDivs+1)+8-1)/8,
               dominancesSz = ((qtDivs+8-1)/8);
  #ifdef DEBUG_ON 
    Serial.println(F("==========ZONEVALS=========="));
    Serial.print(F("sz: ")); Serial.println(zoneValsSz); 
  
    for(int i=0;i<zoneValsSz;i++) Serial.println(zoneVals[i]);

    Serial.println(F("==========DOMINANCES=========="));
    Serial.print(F("sz: ")); Serial.println(dominancesSz); 
  
    for(int i=0;i<dominancesSz;i++)
      Serial.println(dominances[i]);
  #endif
}

LdVarDevice<IO_IN_AL_GEN>::~LdVarDevice(){
  delete[] divs;
  delete[] zoneVals;
  delete[] dominances;
}

uint8_t LdVarDevice<IO_IN_AL_GEN>::getValue(){
  if(device==nullptr) return LdVar::getValue();
  uint16_t k = map(device->read(), 0, 1023, 0, 500);

  uint16_t sg = smallest_greater<uint16_t>(divs, qtDivs, k);
  
  #ifdef DEBUG_ON
    Serial.print(F("Analog PIN: ")); Serial.println(device->getPin());
    Serial.print(F("SG: ")); Serial.println(sg);
    Serial.print(F("divs[")); Serial.print(sg); Serial.print(F("]: "));
    Serial.println(divs[sg]);

    if(sg==qtDivs || divs[sg]!=k) return get_bit_arr(zoneVals, sg, qtDivs+1);  //zoneVals[sg]
    else{
      Serial.println(F("Verificando indicador de dominancia!!!"));
      if(get_bit_arr(dominances, sg, qtDivs)==0){
        Serial.println(F("Esquerda ganhou"));
        return get_bit_arr(zoneVals, sg, qtDivs+1);  //zoneVals[sg]
      }
      else{
        Serial.println(F("Direita ganhou"));
        return get_bit_arr(zoneVals, sg+1, qtDivs+1); //zoneVals[sg+1]
      }
    }
  #else
    return (sg==qtDivs || divs[sg]!=k || get_bit_arr(dominances, sg, qtDivs)==0)
      ? get_bit_arr(zoneVals, sg, qtDivs+1)  //zoneVals[sg]
      : get_bit_arr(zoneVals, sg+1, qtDivs+1); //zoneVals[sg+1];
  #endif
}
//=======================================================

//================LdVarDevice<IO_OUT_DG_GEN>=================
template<>
uint8_t LdVarDevice<IO_OUT_DG_GEN>::getValue(){
  if(device==nullptr) return LdVar::getValue();

  uint8_t val = device->read();

  #ifdef DEBUG_ON
    Serial.print(F("Pino: ")); Serial.print(device->getPin());
    Serial.print(F(" // Valor lido: ")); Serial.println(val);
  #endif

  return val;
}

template<>
void LdVarDevice<IO_OUT_DG_GEN>::setValue(uint8_t value){
  if(device==nullptr) return LdVar::setValue(value);

  #ifdef DEBUG_ON
    Serial.print(F("Pino: ")); Serial.print(device->getPin());
    Serial.print(F(" // Valor escrito: ")); Serial.println(value);
  #endif
  
  device->write(value);
}
//=======================================================
