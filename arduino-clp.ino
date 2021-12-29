#include "Protocol.h"
#include "env.h"

Protocol ptc;

const sz_ptc sz = 100;
uint8_t protocol[sz] = {};

void setup(){
  ptc.set_protocol(protocol, sz);
}

void loop(){
  ptc.run_diag();
}
