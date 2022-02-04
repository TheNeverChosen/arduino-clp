#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "plcIO.h"
#include "Ladder.h"
#include "plc.h"
#include "utils.h"
#include "env.h"

typedef unsigned long long ull;

Plc::Plc():deviceArr(), ldVarArr(), diagram(NULL), qtDevs(0), qtVars(0), ptcSz(0), diagSz(0), ready(false){
  reset_protocol();
}

////////////////////////criação dos dispositivos///////////////////////
DeviceBase* Plc::create_device(IOTypeModel tpMd, uint8_t doorId){
  DeviceBase *dev;
  switch(tpMd){
    /*===================input digital genérico===================*/
    case IO_IN_DG_GEN:{
      Device<IO_IN_DG_GEN> *dev = new Device<IO_IN_DG_GEN>(doorId);
      return static_cast<DeviceBase*>(dev);}
    
    /*===================input analógico genérico==================*/  
    case IO_IN_AL_GEN:{
      Device<IO_IN_AL_GEN> *dev = new Device<IO_IN_AL_GEN>(doorId);
      return static_cast<DeviceBase*>(dev);}

    /*===================output digital genérico===================*/
    case IO_OUT_DG_GEN:{
      Device<IO_OUT_DG_GEN> *dev = new Device<IO_OUT_DG_GEN>(doorId);
      return static_cast<DeviceBase*>(dev);}
      
    default:
      return new DeviceBase();
  }
}


///////////////////////////////////////criação de variaveis//////////////////////////////////////
LdVar* Plc::create_ld_var(IOTypeModel tpMd, DeviceBase *dev, uint8_t *protocol, sz_ptc &i){
  sz_varr varId = consume_bytes<sz_varr>(protocol, i);

  switch(tpMd){
    case IO_IN_DG_GEN:{
      LdVarDevice<IO_IN_DG_GEN> *var = new LdVarDevice<IO_IN_DG_GEN>(varId, dev);
      return static_cast<LdVar*>(var);}
    case IO_IN_AL_GEN:{
      uint16_t qtDivs = consume_bytes<uint16_t,sz_ptc>(protocol, i);
      Serial.print("qtDivs : ");Serial.println(qtDivs);
      
      uint16_t *divs = new uint16_t[qtDivs];

      for(uint8_t j=0;j<qtDivs;j++){
        divs[j] = consume_bytes<uint16_t,sz_ptc>(protocol, i);

        Serial.print(F("divs [")); Serial.print(j); Serial.print(F("]: "));
        Serial.println(divs[j]);
      }

      uint16_t zoneValsSz = ((qtDivs+1)+8-1)/8,
               dominancesSz = (qtDivs+8-1)/8;
      uint8_t *zoneVals = new uint8_t[zoneValsSz],
        *dominances = new uint8_t[dominancesSz];
      
      Serial.print("zoneValsSz: "); Serial.print(zoneValsSz); Serial.print(" / ");
      Serial.print("dominancesSz: "); Serial.print(dominancesSz); Serial.println();

      Serial.print("Entering zoneVals at index "); Serial.println(i); Serial.println();
      for(uint16_t j=0;j<zoneValsSz;j++){
        zoneVals[j] = protocol[i++];
        Serial.print(F("zoneVals[")); Serial.print(j); Serial.print(F("]: "));
        Serial.println(zoneVals[j]);
      }
      
      Serial.print("Entering dominances at index "); Serial.println(i); Serial.println();
      for(uint16_t j=0;j<dominancesSz;j++){
        dominances[j] = protocol[i++];
        Serial.print(F("dominances[")); Serial.print(j); Serial.print(F("]: "));
        Serial.println(dominances[j]);
      }

      Serial.print(F("Final I: ")); Serial.println(i);

      LdVarDevice<IO_IN_AL_GEN> *var = new LdVarDevice<IO_IN_AL_GEN>(varId, dev, qtDivs, divs, zoneVals, dominances);
     
      Serial.println(F("var IO_IN_AL_GEN var ok"));
     
      return var;}
    case IO_OUT_DG_GEN:{
      LdVarDevice<IO_OUT_DG_GEN> *var = new LdVarDevice<IO_OUT_DG_GEN>(varId, dev);
      return static_cast<LdVar*>(var);}
    default:
      return new LdVar();
  }
}

uint8_t* Plc::getProtocol(){
  return protocol;
}

///////////////////////reset_protocol/////////////////////////
////////zerar antigos valores dos arrays e variaveis////////
///////////do protocol embarcado anteriormente/////////////
void Plc::reset_protocol(uint8_t *protocol, sz_ptc sz){
  for(int i=0;i<qtDevs;i++){
    delete deviceArr[i];
    deviceArr[i]=NULL;
  }
  for(int i=0;i<qtVars;i++){
    delete ldVarArr[i];
    ldVarArr[i]=NULL;
  }
  for(int i=0;i<QT_OUT_DG;i++){
    pinMode(outDgPins[i], OUTPUT);    //making sure it's set as output pin
    digitalWrite(outDgPins[i], LOW);  //setting output value to LOW (Default)
  }
  for(int i=0;i<QT_IN_DG;i++)
    pinMode(inDgPins[i], INPUT);  //making sure it's set as input pin

  qtDevs=qtVars=diagSz=ptcSz=0;
  diagram=NULL;
  ready=false;

  if(protocol!=nullptr && sz>0){
    memcpy(this->protocol, protocol, sizeof(uint8_t) * sz);
    ptcSz = sz;
    diagSz = 0;
  }
}

bool Plc::isReady(){
  return ready;
}

void Plc::set_ptcSz(sz_ptc ptcSz){
  this->ptcSz = ptcSz;
}


///////////////////////////set_dev_vars_diag///////////////////////////
//////////////////////////////configurar://////////////////////////////
/////////////////////////devices(dispositivos),////////////////////////
/////////////////////////variables(variaveis e/////////////////////////
////////////////////////diagram(diagrama ladder)///////////////////////
void Plc::set_dev_vars_diag(){
  if(ptcSz==0)  return;

  sz_ptc i=0;

  qtDevs = protocol[i++]; //quantidade de dispositivos (1 bytes)
  
  Serial.println(F("set_dev_vars_diag iniciado"));
  
  qtVars = consume_bytes<sz_varr>(protocol, i); //quantidade de variaveis (2 bytes)

  Serial.print(F("QtDevs: ")); Serial.println(qtDevs);
  Serial.print(F("QtVars: ")); Serial.println(qtVars);

  for(sz_varr j=0;j<qtDevs;j++){
    
    Serial.print(F("Lendo o device: ")); Serial.println(j);
    
    IOTypeModel tpMd = (IOTypeModel) protocol[i++];
    uint8_t doorId = protocol[i++];
    sz_varr qtVarDev = consume_bytes<sz_varr>(protocol, i);
    deviceArr[j] = create_device(tpMd, doorId);

    Serial.print(F("Lendo variaveis. QtVarDev: ")); Serial.println(qtVarDev);
    
    while(qtVarDev--){
      LdVar *var = create_ld_var(tpMd, deviceArr[j], protocol, i);
      ldVarArr[var->getId()] = var;
    }
  }

  Serial.println(F("Inicializando variaveis internas (NULL)"));
  
  for(sz_varr j=0;j<qtVars;j++)
    if(ldVarArr[j] == NULL)
      ldVarArr[j] = new LdVarInternal(j);
  
  Serial.println(F("Variaveis internas inicializadas"));

  diagram = &protocol[i];
  diagSz = ptcSz-i;
  
  Serial.print(F("ptcSz: ")); Serial.println(ptcSz); 
  Serial.print(F("DiagSz: ")); Serial.println(diagSz);

  ready=true;
}

bool isRelayComp(uint8_t x){
  return (x>RC_BEG && x<RC_END);
}

////////////////////////exec_fork///////////////////////
/////////////função para leitura do paralelo////////////
bool Plc::exec_fork(sz_ptc &i){
  if(diagram[i]!=F1) return false;

  bool res = false;

  #ifdef DEBUG_ON
    Serial.println(F("==========EXECUTANDO FORK=========="));
    Serial.print(F("I: ")); Serial.println(i);
    Serial.println(F("===================================="));
  #endif

  while(diagram[i]!=F2){
    if(diagram[i]==P1) res = exec_path(i, P2) || res;
    else i++;
  }
  
  #ifdef DEBUG_ON
    Serial.println(F("==========TERMINANDO FORK=========="));
    Serial.print(F("I: ")); Serial.println(i);
    Serial.print(F("Res: ")); Serial.println(res);
    Serial.println(F("===================================="));
  #endif
  
  i++;
  return res;
}

//////////////////////////exec_path/////////////////////////
///função para leitura de linhas principais e do paralelo///

bool Plc::exec_path(sz_ptc &i, uint8_t stopAt){
  if(diagram[i]!=L1 && diagram[i]!=P1) return false;

  bool res=true;

  #ifdef DEBUG_ON
    Serial.println(F("==========EXECUTANDO LINHA=========="));
    Serial.print(F("I: ")); Serial.println(i);
    Serial.print(F("StopAt: ")); Serial.println(stopAt);
    Serial.println(F("===================================="));
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
    Serial.println(F("==========TERMINANDO LINHA=========="));
    Serial.print(F("I: ")); Serial.println(i);
    Serial.print(F("Res: ")); Serial.println(res);
    Serial.println(F("===================================="));
  #endif
  
  i++;
  return res;
}


///////função para executar o diagrama///////
void Plc::run_diag(){
  if(!ready){
    #ifdef DEBUG_ON
      Serial.println(F("PLC not ready to run."));
    #endif
    return;
  }
  
  #ifdef DEBUG_ON
    Serial.println(F("\n\n==========RUN DIAGRAM==========\n\n"));
  #endif

  for(sz_ptc i=0;i<diagSz;){
    if(diagram[i]==L1) exec_path(i, L2);
    else i++;
  }
  #ifdef DEBUG_ON
    Serial.println(F("\n\n==========RUN DIAGRAM DONE==========\n\n"));
  #endif
}