#include "Protocol.h"
#include "env.h"

Protocol ptc;

uint8_t protocol[] = {
  6, 6, //qtDevs, qtVars)

  101, 0, 1, //IO_IN_AL_GEN, Porta 0 (pin A0), qtVarDev (1)
  0, //varId (0) POT 0
  2,0, //qtDivs
  200, 0, 144, 1, //divisores (2)
  160, //10100000 -> intervalos (3)
  64, //01000000 -> dominancia (2)

  101, 1, 1, //IO_IN_AL_GEN, Porta 1 (pin A1), qtVarDev (1)
  1, //varId (1) POT 1
  2,0, //qtDivs
  200, 0, 144, 1, //divisores (2)
  160, //10100000 -> intervalos (3)
  64, //01000000 -> dominancia (2)

  1, 0, 1, //IO_IN_DG_GEN, Porta 0 (pin 27), qtVarDev (1)
  2, //varId (2) BOTAO - ON

  1, 1, 1, //IO_IN_DG_GEN, Porta 0 (pin 27), qtVarDev (1)
  3, //varId (3) BOTAO - OFF

  201, 0, 1, //IO_OUT_DG, Porta 0 (pin 41), qtVarDev(1)
  4, //varId (4) LED TURN ON

  201, 1, 1, //IO_OUT_DG, Porta 1 (pin 43), qtVarDev(1)
  5, //varId (5) LED

  L1,  F1,  P1, CA,2, P2,  P1, CA, 4, P2,  F2,  CF,3  ,BA, 4, L2,
  L1, CA,4,  F1,  P1, CA, 0, CF,1, P2,  P1, CF,0 ,CA,1 ,P2 ,F2,  BA,5, L2};

const sz_ptc sz = sizeof(protocol);

void setup(){
  Serial.begin(9600);
  ptc.set_protocol(protocol, sz);
  ptc.set_dev_vars_diag();
}

bool first=true;

void loop(){
  ptc.run_diag();
}
