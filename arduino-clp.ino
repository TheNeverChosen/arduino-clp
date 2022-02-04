#include <Arduino.h>
#include "connect.h"
#include "env.h"

Connect connect;

void setup(){
  Serial.begin(BAUD_RATE);
  while(!Serial){}
  connect.begin_network();
}

void loop(){
  connect.websocket();
  connect.protocol_run_diag();
}