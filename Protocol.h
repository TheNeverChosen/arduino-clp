#pragma once

#include <stdint.h>
#include "CLPIO.h"
#include "Ladder.h"
#include "env.h"

class Protocol{
private:
  DeviceBase *deviceArr[QT_MAX_DEV];
  LdVar *ldVarArr[QT_MAX_LD_VAR];
  uint8_t protocol[MAX_SZ_PROTOCOL], *diagram, qtDevs;
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

  void set_protocol(uint8_t *protocol, sz_ptc sz);
  void set_dev_vars_diag();
  void run_diag();
  
};