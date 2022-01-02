#include "Protocol.h"
#include "env.h"

Protocol ptc;

const sz_ptc sz = 23;
uint8_t protocol[sz] = {
  2, 2, 0, //qtDevs, qtVars
  
  1, 0,  1, 0, //IO_IN_DG_GEN, Porta 0 (pin 22), qtVarDev (1)
  0, 0, //varId (0)
  
  201, 0,  1, 0, //IO_OUT_DG, Porta 0 (pin 28), qtVarDEv(1)
  1, 0, //varId (1)
  
  1,11,0,0,13,1,0,2};

//uint8_t protocol[sz] = {10,10,1,0,1,0,11001001,0,1,1,1,1011,0,1011,1,10};

void setup(){
  Serial.begin(9600);
  ptc.set_protocol(protocol, sz);
  ptc.set_dev_vars_diag();
}

void loop(){
  ptc.run_diag();
  //delay(1000);
}
