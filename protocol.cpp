#include <Arduino.h>
#include <stdint.h>
#include "CLPIO.h"
#include "Ladder.h"
#include "env.h"

#define QT_MAX_LD_VAR 1024

typedef unsigned long long ull;

const unsigned qtTotalDev = QT_INP_ALG + QT_INP_DIG + QT_OUT_DIG;
DeviceBase *deviceArr[qtTotalDev];

LdVar *ldVarArr[QT_MAX_LD_VAR]={};

template<typename T>
T consume_bytes(uint8_t *arr, ull &i){
  T res = *((T*)(&arr[i]));
  i+=sizeof(T);
  return res;
}

DeviceBase* create_device(IOTypeModel tpMd, uint8_t doorId){
  DeviceBase *dev;
  switch(tpMd){
    case IO_IN_DG_GEN:{
      Device<IO_IN_DG_GEN> *dev = new Device<IO_IN_DG_GEN>(doorId);
      return static_cast<DeviceBase*>(dev);}
    case IO_IN_AL_GEN:{
      Device<IO_IN_AL_GEN> *dev = new Device<IO_IN_AL_GEN>(doorId);
      return static_cast<DeviceBase*>(dev);}
    case IO_OUT_DG:{
      Device<IO_OUT_DG> *dev = new Device<IO_OUT_DG>(doorId);
      return static_cast<DeviceBase*>(dev);}
    default:
      return new DeviceBase();
  }
}

LdVar* create_ld_var(IOTypeModel tpMd, DeviceBase *dev, uint8_t *protocol, ull &i){
  uint16_t varId = consume_bytes<uint16_t>(protocol, i);

  switch(tpMd){
    case IO_IN_DG_GEN:{
      LdVarDevice<IO_IN_DG_GEN> *var = new LdVarDevice<IO_IN_DG_GEN>(varId, dev);
      return static_cast<LdVar*>(var);}
    case IO_IN_AL_GEN:{
      uint16_t qtDivs = consume_bytes<uint16_t>(protocol, i);
      uint16_t *divs = new uint16_t[qtDivs];

      for(uint8_t j=0;j<qtDivs;j++)
        divs[j] = consume_bytes<uint16_t>(protocol, i);

      uint16_t zoneValsSz = ((qtDivs+1)+8-1)/8,
               dominancesSz = ((qtDivs+8-1)/8);
      uint8_t *zoneVals = new uint8_t[zoneValsSz],
        *dominances = new uint8_t[dominancesSz];
      
      for(uint16_t j=0;j<zoneValsSz;j++)
        zoneVals[j] = protocol[i++];
      
      for(uint16_t j=0;j<dominancesSz;j++)
        dominances[j] = protocol[i++];

      LdVarDevice<IO_IN_AL_GEN> *var = new LdVarDevice<IO_IN_AL_GEN>(varId, dev, qtDivs, divs, zoneVals, dominances);
      
      return var;}
    case IO_OUT_DG:{
      LdVarDevice<IO_OUT_DG> *var = new LdVarDevice<IO_OUT_DG>(varId, dev);
      return static_cast<LdVar*>(var);}
    default:
      return new LdVar();
  }
}

void read_protocol(uint8_t *protocol, unsigned sz){
  ull i=0;
  
  uint8_t qtD = protocol[i++]; //quantidade de dispositivos (1 bytes)
  uint16_t qtV = consume_bytes<uint16_t>(protocol, i); //quantidade de variaveis (2 bytes)
  
  for(uint8_t j=0;j<qtD;j++){
    IOTypeModel tpMd = (IOTypeModel) protocol[i++];
    uint8_t doorId = protocol[i++];
    uint16_t qtVar = consume_bytes<uint16_t>(protocol, i); 
    deviceArr[j] = create_device(tpMd, doorId);

    for(uint16_t k=0;k<qtVar;k++)
      ldVarArr[k] = create_ld_var(tpMd, deviceArr[j], protocol, i);
  }

  for(uint8_t j=qtV;j<qtV;j++)
      if(ldVarArr[j] == NULL)
        ldVarArr[j] = new LdVarInternal;
  
}