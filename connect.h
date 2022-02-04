#pragma once

#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include <stdint.h>
#include "plc.h"

enum PlcRunStates{PLC_RUN_ST_IDLE=0, PLC_RUN_ST_SETTING, PLC_RUN_ST_PAUSED, PLC_RUN_ST_RUNNING};

enum WsConnectState{WS_CON_ST_NEVER=-1, WS_CON_ST_DISCONNECTED=0, WS_CON_ST_CONNECTED=1};

class Connect{
private:
  Plc plc;
  EthernetClient etClient;
  WebSocketClient wsClient;
  enum WsConnectState lastConSt;
  enum PlcRunStates plcRunSt;

  bool websocket_connect();
  void setLastConSt(enum WsConnectState conSt);
  void setPlcRunSt(enum PlcRunStates st);
  void websocket_msg_handler();

public:
  Connect();
  void begin_network();
  void websocket();
  void update_led_monitor();
  void protocol_run_diag();
};