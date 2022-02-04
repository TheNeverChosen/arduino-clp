#include <Arduino.h>
#include <stdlib.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "connect.h"
#include "plc.h"
#include "led_monitor.h"
#include "utils.h"
#include "env.h"

enum WsBinMsgCode{WS_BIN_IDENTIFICATION=0,WS_BIN_PLC_PROTOCOL=1,WS_BIN_PLC_CONTROL=2};

enum PlcControlCodes{PLC_CONTROL_PAUSE=0, PLC_CONTROL_RESUME=1, PLC_CONTROL_RESET=2};

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);
char serverAddress[] = "34.95.137.51";
int port = 80;

Connect::Connect():plc(), etClient(), wsClient(etClient,serverAddress, port), lastConSt(WS_CON_ST_NEVER), plcRunSt(PLC_RUN_ST_IDLE){
  led_monitor_reset();
}

void Connect::begin_network(){
  if(Ethernet.begin(mac) == 0){
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
  }
}

void Connect::setLastConSt(enum WsConnectState lastConSt){
  this->lastConSt=lastConSt;
  led_monitor_set_wsConnection(
    lastConSt==WS_CON_ST_CONNECTED
      ? true
      : false
  );
}

void Connect::setPlcRunSt(enum PlcRunStates runSt){
  led_monitor_set_run(plcRunSt=runSt);
}

bool Connect::websocket_connect(){
  if(wsClient.connected()){
    setLastConSt(WS_CON_ST_CONNECTED);
    return true;
  }

  if(lastConSt==WS_CON_ST_NEVER)
    Serial.println(F("Trying to connect to WebSocket server..."));
  else if(lastConSt==WS_CON_ST_CONNECTED)
    Serial.println(F("Disconnected from WebSocket server. Trying to reconnect..."));
  
  setLastConSt(WS_CON_ST_DISCONNECTED);

  wsClient.begin(F("/arduino"));
  
  if(!wsClient.connected()) return false; //couldn't reconnect

  Serial.println(F("Connected to WebSocket server."));
  setLastConSt(WS_CON_ST_CONNECTED);

  uint8_t plcVersion[3];
  get_version(plcVersion);

  wsClient.beginMessage(TYPE_BINARY);
  wsClient.write(0);
  wsClient.write(plcVersion, 3);
  wsClient.write(get_reference());
  wsClient.endMessage();

  return true;
}

void Connect::websocket_msg_handler(){
  int msgSz = wsClient.parseMessage();
  if(msgSz<=0) return;

  #ifdef DEBUG_ON
    Serial.print("MESSAGE RECEIVED. SIZE: ");
    Serial.println(msgSz);
  #endif

  if(wsClient.messageType()==TYPE_TEXT){
    Serial.println(F("Received text message:"));
    Serial.println(wsClient.readString());
    return;
  }
  if(wsClient.messageType()!=TYPE_BINARY) return;

  uint8_t msgCode = wsClient.read(); msgSz--;

  switch(msgCode){
    case WS_BIN_PLC_PROTOCOL:{
      if(msgSz<2){
        Serial.println(F("ERROR: PLC Protocol message too short!"));
        break;
      }

      uint8_t ptcSzArr[sizeof(sz_ptc)];
      wsClient.read(ptcSzArr, sizeof(sz_ptc)); //reading ptcSz
      msgSz-=sizeof(sz_ptc);
      sz_ptc ptcSz = consume_bytes<sz_ptc>(ptcSzArr);

      if(ptcSz>MAX_SZ_PROTOCOL){
        Serial.println(F("ERROR: PLC Protocol too long."));
        break;
      }
      if(msgSz<ptcSz){
        Serial.println(F("ERROR: PLC Protocol too short for informed size."));
        break;
      }

      uint8_t *protocol = plc.getProtocol();
      setPlcRunSt(PLC_RUN_ST_IDLE);
      plc.reset_protocol(); //reseting protocol
      plc.set_ptcSz(ptcSz); //setting protocol Size
      wsClient.read(protocol, ptcSz); //reading and setting protocol

      #ifdef DEBUG_ON
        Serial.println(F("==========Received Protocol=========="));
        Serial.print(F("Size: ")); Serial.println(ptcSz);
        for(sz_ptc i=0;i<ptcSz;i++){
          Serial.print(protocol[i]);
          Serial.print(F(" "));
        }
        Serial.println();
      #endif

      plc.set_dev_vars_diag(); //setting devices, variables and diagram
      setPlcRunSt(PLC_RUN_ST_RUNNING); //resuming plc execution

      break;
    }
    case WS_BIN_PLC_CONTROL:{
      if(msgSz<=0){
        Serial.println(F("ERROR: PLC Control message too short!"));
        break;
      }

      uint8_t controlCode = wsClient.read();

      if(controlCode==PLC_CONTROL_RESET){
        setPlcRunSt(PLC_RUN_ST_IDLE);
        plc.reset_protocol(); //reseting protocol
      }
      else if(plcRunSt>PLC_RUN_ST_SETTING){ //checking running capability
        if(controlCode==PLC_CONTROL_PAUSE) setPlcRunSt(PLC_RUN_ST_PAUSED);
        else if(controlCode==PLC_CONTROL_RESUME) setPlcRunSt(PLC_RUN_ST_RUNNING);
      }

      break;
    }
    default: break;
  }
}

void Connect::websocket(){
  if(!websocket_connect()) return; //assegurar conexao
  websocket_msg_handler();
}

void Connect::protocol_run_diag(){
  if(plcRunSt==PLC_RUN_ST_RUNNING) plc.run_diag();
}