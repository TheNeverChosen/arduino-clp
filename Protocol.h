#pragma once

#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "CLPIO.h"
#include "Ladder.h"
#include "env.h"

///////////typedef da estrutura do diagrama///////////
////////linhas, paralelo e linhas do paralelo/////////
typedef enum{
  L1=1, L2=2, F1=3, F2=4, P1=5, P2=6 
}Symbols;

///////componentes dos relés: contatos e bobinas//////
typedef enum{
  RC_BEG=10, CA=11, CF=12, BA=13, BF=14, RC_END=15
}RelayComps;

class Protocol{
private:
  uint8_t protocol[MAX_SZ_PROTOCOL];
  DeviceBase *deviceArr[QT_MAX_DEV];
  LdVar *ldVarArr[QT_MAX_LD_VAR];
  uint8_t  *diagram, qtDevs;
  sz_varr qtVars;
  sz_ptc ptcSz, diagSz;

  template<typename T>
  T consume_bytes(uint8_t *arr, sz_ptc &i);
  DeviceBase* create_device(IOTypeModel tpMd, uint8_t doorId);
  LdVar* create_ld_var(IOTypeModel tpMd, DeviceBase *dev, uint8_t *protocol, sz_ptc &i);

  bool exec_fork(sz_ptc &i);
  bool exec_path(sz_ptc &i, uint8_t stopAt);

public:
  Protocol();

  uint8_t* getProtocol();
  void reset_protocol(uint8_t *protocol=nullptr, sz_ptc sz=0);

  void set_dev_vars_diag();
  void run_diag();
  void readWsProtocol(WebSocketClient &wsClient);

};

