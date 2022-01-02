#include <Arduino.h>
#include <stdint.h>
#include "CLPIO.h"
#include "Ladder.h"
#include "Protocol.h"
#include "env.h"

typedef unsigned long long ull;

Protocol::Protocol():deviceArr(), ldVarArr(), diagram(NULL), qtDevs(0), qtVars(0), ptcSz(0), diagSz(0){ }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T Protocol::consume_bytes(uint8_t *arr, sz_ptc &i){
  //Serial.print("consume_bytes indice/valor: ");
  //Serial.print(i); Serial.print(" / "); Serial.println(arr[i+1]);
  T res = *((T*)(&arr[i]));
  i+=sizeof(T);
  //Serial.println(i);
  //Serial.print("consume_bytes ok: "); Serial.println(res);
  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

DeviceBase* Protocol::create_device(IOTypeModel tpMd, uint8_t doorId){
  DeviceBase *dev;
  switch(tpMd){
    case IO_IN_DG_GEN:{
      Device<IO_IN_DG_GEN> *dev = new Device<IO_IN_DG_GEN>(doorId);
      Serial.println("device IO_IN_DG_GEN ok");
      return static_cast<DeviceBase*>(dev);}
    case IO_IN_AL_GEN:{
      Device<IO_IN_AL_GEN> *dev = new Device<IO_IN_AL_GEN>(doorId);
      Serial.println("device IO_IN_AL_GEN ok");
      return static_cast<DeviceBase*>(dev);}
    case IO_OUT_DG:{
      Device<IO_OUT_DG> *dev = new Device<IO_OUT_DG>(doorId);
      Serial.println("device IO_OUT_DG ok");
      return static_cast<DeviceBase*>(dev);}
    default:
      return new DeviceBase();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

LdVar* Protocol::create_ld_var(IOTypeModel tpMd, DeviceBase *dev, uint8_t *protocol, sz_ptc &i){
  sz_varr varId = consume_bytes<uint16_t>(protocol, i);

  switch(tpMd){
    case IO_IN_DG_GEN:{
      LdVarDevice<IO_IN_DG_GEN> *var = new LdVarDevice<IO_IN_DG_GEN>(varId, dev);
      Serial.println("var IO_IN_DG_GEN ok");
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
      Serial.println("var IO_IN_AL_GEN var ok");
      return var;}
    case IO_OUT_DG:{
      LdVarDevice<IO_OUT_DG> *var = new LdVarDevice<IO_OUT_DG>(varId, dev);
      Serial.println("var IO_OUT_DG var ok");
      return static_cast<LdVar*>(var);}
    default:
      return new LdVar();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Protocol::set_protocol(uint8_t *protocol, sz_ptc sz){
  for(int i=0;i<qtDevs;i++) delete deviceArr[i];  
  for(int i=0;i<qtVars;i++) delete ldVarArr[i];
  
  qtDevs=0; qtVars=0;

  memcpy(this->protocol, protocol, sizeof(uint8_t) * sz);

  ptcSz = sz;
  diagSz = 0;
  diagram=NULL;
  Serial.println("protocolo setado");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Protocol::set_dev_vars_diag(){
  if(ptcSz==0){Serial.println("ptcSz é 0"); return;}

  sz_ptc i=0;

  qtDevs = protocol[i++]; //quantidade de dispositivos (1 bytes)
  Serial.println("set_dev_vars_diag iniciado");
  qtVars = consume_bytes<sz_varr>(protocol, i); //quantidade de variaveis (2 bytes)

  Serial.print("QtDevs: "); Serial.println(qtDevs);
  Serial.print("QtVars: "); Serial.println(qtVars);

  for(sz_varr j=0;j<qtDevs;j++){
    Serial.print("Lendo device: "); Serial.println(j);
    IOTypeModel tpMd = (IOTypeModel) protocol[i++];
    uint8_t doorId = protocol[i++];
    sz_varr qtVarDev = consume_bytes<sz_varr>(protocol, i);
    deviceArr[j] = create_device(tpMd, doorId);

    Serial.print("Lendo variaveis. QtVarDev: "); Serial.println(qtVarDev);
    while(qtVarDev--){
      LdVar *var = create_ld_var(tpMd, deviceArr[j], protocol, i);
      ldVarArr[var->getId()] = var;
      Serial.print("Lendo variavel. Faltam: "); Serial.println(qtVarDev);
    }
  }

  Serial.println("Inicializando variaveis internas (NULL)");
  for(sz_varr j=0;j<qtVars;j++)
    if(ldVarArr[j] == NULL)
      ldVarArr[j] = new LdVarInternal(j);
  Serial.println("Variaveis internas inicializadas");

  diagram = &protocol[i];
  diagSz = ptcSz-i;
  Serial.print("ptcSz: "); Serial.println(ptcSz); 
  Serial.print("DiagSz: "); Serial.println(diagSz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum{
  L1=1, L2=2, F1=3, F2=4, P1=5, P2=6 
}Symbols;
typedef enum{
  RC_BEG=10, CA=11, CF=12, BA=13, BF=14, RC_END=15
}RelayComps;
bool isRelayComp(uint8_t x){
  return (x>RC_BEG && x<RC_END);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Protocol::exec_fork(sz_ptc &i){
  bool res = false;
  while(diagram[i]!=F2){
    if(diagram[i]==P1) res = exec_path(++i, P2) || res;
    else i++;
  }
  Serial.println("funcao de leitura de paralelo ok");
  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Protocol::exec_path(sz_ptc &i, uint8_t stopAt){
  bool res=true;

  while(i<diagSz && diagram[i]!=stopAt){
    if(isRelayComp(diagram[i])){
      RelayComps x = (RelayComps) diagram[i++];
      sz_varr iVar = consume_bytes<uint16_t>(diagram, i);

      switch(x){
        case CA:
          res = (res && ldVarArr[iVar]->getValue()); break;
        case CF:
          res = (res && !ldVarArr[iVar]->getValue()); break;
        case BA:
          ldVarArr[iVar]->setValue(res); break;
        case BF:
          ldVarArr[iVar]->setValue(!res); break;
      }
    }
    else if(diagram[i]==F1) res = res && exec_fork(++i);
  }

  i++;
  Serial.println("funcao de leitura de linha ok");
  return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Protocol::run_diag(){
  if(diagram==NULL){Serial.println("diagram é NULL");return;}
   
  //Serial.println("started run_diag");

  for(sz_ptc i=0;i<diagSz;){
    if(diagram[i]==L1) exec_path(++i, L2);
    else i++;
  }
}
