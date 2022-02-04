#pragma once

#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include "plc.h"

class Connect{
private:
  Plc plc;
  EthernetClient etClient;
  WebSocketClient wsClient;
  bool disconnecting;

  bool websocket_connect();
  void websocket_msg_handler();

public:
  void Connect();
  void protocol_run_diag();
  void begin_network();
  void websocket();
};