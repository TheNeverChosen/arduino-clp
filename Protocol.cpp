#include <Arduino.h>
#include <stdint.h>
#include "CLPIO.h"
#include "Ladder.h"
#include "Protocol.h"
#include "env.h"

typedef unsigned long long ull;

Protocol::Protocol():deviceArr(), ldVarArr(), diagram(NULL), qtDevs(0), qtVars(0), ptcSz(0), diagSz(0){ }

////////////////////////////comsume_bytes////////////////////////////
///////////////////pegar duas posições de um array///////////////////
template<typename T>
T Protocol::consume_bytes(uint8_t *arr, sz_ptc &i){
  T res = *((T*)(&arr[i]));
  i+=sizeof(T);
  return res;
}


/////////////////////////////create_device/////////////////////////////
////////////////////////criação dos dispositivos///////////////////////
DeviceBase* Protocol::create_device(IOTypeModel tpMd, uint8_t doorId){
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


//////////////////////////////////////////create_ld_var//////////////////////////////////////////
///////////////////////////////////////criação de variaveis//////////////////////////////////////
LdVar* Protocol::create_ld_var(IOTypeModel tpMd, DeviceBase *dev, uint8_t *protocol, sz_ptc &i){
  sz_varr varId = consume_bytes<sz_varr>(protocol, i);

  switch(tpMd){
    case IO_IN_DG_GEN:{
      LdVarDevice<IO_IN_DG_GEN> *var = new LdVarDevice<IO_IN_DG_GEN>(varId, dev);
      return static_cast<LdVar*>(var);}
    case IO_IN_AL_GEN:{
      uint16_t qtDivs = consume_bytes<uint16_t>(protocol, i);
      Serial.print("qtDivs : ");Serial.println(qtDivs);
      
      uint16_t *divs = new uint16_t[qtDivs];

      for(uint8_t j=0;j<qtDivs;j++){
        divs[j] = consume_bytes<uint16_t>(protocol, i);

        Serial.print("divs ["); Serial.print(j); Serial.print("]: ");
        Serial.println(divs[j]);
      }

      uint16_t zoneValsSz = ((qtDivs+1)+8-1)/8,
               dominancesSz = ((qtDivs+8-1)/8);
      uint8_t *zoneVals = new uint8_t[zoneValsSz],
        *dominances = new uint8_t[dominancesSz];
      
      for(uint16_t j=0;j<zoneValsSz;j++){
        zoneVals[j] = protocol[i++];
        Serial.print("zoneVals["); Serial.print(j); Serial.print("]: ");
        Serial.println(zoneVals[j]);
      }
      
      for(uint16_t j=0;j<dominancesSz;j++){
        dominances[j] = protocol[i++];
        Serial.print("dominances["); Serial.print(j); Serial.print("]: ");
        Serial.println(dominances[j]);
      }

      Serial.print("Final I: "); Serial.println(i);

      LdVarDevice<IO_IN_AL_GEN> *var = new LdVarDevice<IO_IN_AL_GEN>(varId, dev, qtDivs, divs, zoneVals, dominances);
     
      Serial.println("var IO_IN_AL_GEN var ok");
     
      return var;}
    case IO_OUT_DG:{
      LdVarDevice<IO_OUT_DG> *var = new LdVarDevice<IO_OUT_DG>(varId, dev);
      return static_cast<LdVar*>(var);}
    default:
      return new LdVar();
  }
}


///////////////////////set_protocol/////////////////////////
////////zerar antigos valores dos arrays e variaveis////////
///////////do protocolo embarcado anteriormente/////////////
void Protocol::set_protocol(uint8_t *protocol, sz_ptc sz){
  for(int i=0;i<qtDevs;i++) delete deviceArr[i];  
  for(int i=0;i<qtVars;i++) delete ldVarArr[i];
  
  qtDevs=0; qtVars=0;

  memcpy(this->protocol, protocol, sizeof(uint8_t) * sz);

  ptcSz = sz;
  diagSz = 0;
  diagram=NULL;
}


///////////////////////////set_dev_vars_diag///////////////////////////
//////////////////////////////configurar://////////////////////////////
/////////////////////////devices(dispositivos),////////////////////////
/////////////////////////variables(variaveis e/////////////////////////
////////////////////////diagram(diagrama ladder)///////////////////////
void Protocol::set_dev_vars_diag(){
  if(ptcSz==0)  return;

  sz_ptc i=0;

  qtDevs = protocol[i++]; //quantidade de dispositivos (1 bytes)
  
  Serial.println("set_dev_vars_diag iniciado");
  
  qtVars = consume_bytes<sz_varr>(protocol, i); //quantidade de variaveis (2 bytes)

  Serial.print("QtDevs: "); Serial.println(qtDevs);
  Serial.print("QtVars: "); Serial.println(qtVars);

  for(sz_varr j=0;j<qtDevs;j++){
    
    Serial.print("Lendo o device: "); Serial.println(j);
    
    IOTypeModel tpMd = (IOTypeModel) protocol[i++];
    uint8_t doorId = protocol[i++];
    sz_varr qtVarDev = consume_bytes<sz_varr>(protocol, i);
    deviceArr[j] = create_device(tpMd, doorId);

    Serial.print("Lendo variaveis. QtVarDev: "); Serial.println(qtVarDev);
    
    while(qtVarDev--){
      LdVar *var = create_ld_var(tpMd, deviceArr[j], protocol, i);
      ldVarArr[var->getId()] = var;
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

bool isRelayComp(uint8_t x){
  return (x>RC_BEG && x<RC_END);
}

////////////////////////exec_fork///////////////////////
/////////////função para leitura do paralelo////////////
bool Protocol::exec_fork(sz_ptc &i){
  if(diagram[i]!=F1) return false;

  bool res = false;

  #ifdef DEBUG_ON
    Serial.println("==========EXECUTANDO FORK==========");
    Serial.print("I: "); Serial.println(i);
    Serial.println("====================================");
  #endif

  while(diagram[i]!=F2){
    if(diagram[i]==P1) res = exec_path(i, P2) || res;
    else i++;
  }
  
  #ifdef DEBUG_ON
    Serial.println("==========TERMINANDO FORK==========");
    Serial.print("I: "); Serial.println(i);
    Serial.print("Res: "); Serial.println(res);
    Serial.println("====================================");
  #endif
  
  i++;
  return res;
}

//////////////////////////exec_path/////////////////////////
///função para leitura de linhas principais e do paralelo///

bool Protocol::exec_path(sz_ptc &i, uint8_t stopAt){
  if(diagram[i]!=L1 && diagram[i]!=P1) return false;

  bool res=true;

  #ifdef DEBUG_ON
    Serial.println("==========EXECUTANDO LINHA==========");
    Serial.print("I: "); Serial.println(i);
    Serial.print("StopAt: "); Serial.println(stopAt);
    Serial.println("====================================");
  #endif

  while(i<diagSz && diagram[i]!=stopAt){
    if(isRelayComp(diagram[i])){
      RelayComps x = (RelayComps) diagram[i++];
      sz_varr iVar = consume_bytes<sz_varr>(diagram, i);
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
    else if(diagram[i]==F1) res = (exec_fork(i) && res); //i continua o mesmo
    else i++;
  }

  #ifdef DEBUG_ON
    Serial.println("==========TERMINANDO LINHA==========");
    Serial.print("I: "); Serial.println(i);
    Serial.print("Res: "); Serial.println(res);
    Serial.println("====================================");
  #endif
  
  i++;
  return res;
}


///////função para executar o diagrama///////
void Protocol::run_diag(){
  if(diagram==NULL)return;
  
  #ifdef DEBUG_ON
    Serial.println("\n\n==========RUN DIAGRAM==========\n\n");
  #endif

  for(sz_ptc i=0;i<diagSz;){
    if(diagram[i]==L1) exec_path(i, L2);
    else i++;
  }
}