#include "Protocol.h"
#include "env.h"
#include <avr/pgmspace.h>

Protocol ptc;
//const sz_ptc sz = 78;

uint8_t protocol[]  = {

  };

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
