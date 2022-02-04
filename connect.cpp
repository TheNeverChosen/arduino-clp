#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "connect.h"
#include "plc.h"
#include "led_monitor.h"
#include "utils.h"
#include "env.h"

enum WsBinMsgCode{IDENTIFICATION=0,PLC_PROTOCOL=1,PLC_CONTROL=2};

Connect::Connect():plc(), etClient(), wsClient(etClient,serverAddress, port), disconnecting(false){
  set_led_monitor();
}

void Connect::begin_network(){
  if(Ethernet.begin(mac) == 0){
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
  }
}

bool Connect::websocket_connect(){
  if(wsClient.connected()) return true;
  
  wsClient.begin(F("/arduino"));
  
  if(!wsClient.connected()) return false;

  Serial.println(F("Connected to WebSocket server."));

  uint8_t plcVersion[3];
  get_version(plcVersion);

  wsClient.beginMessage(TYPE_BINARY);
  wsClient.write(0);
  wsClient.write(plcVersion, 3);
  wsClient.write(get_reference());
  wsClient.endMessage();
  disconnecting=0;
  return true;
}

void Connect::websocket_flush(){
  while(wsClient.available())
    wsClient.read(); //consuming all left
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

  uint8_t codMsg = wsClient.read(); msgSz--;

  switch(codMsg){
    case PLC_PROTOCOL:
      if(msgSz<=2){
        Serial.println(F("ERROR: PLC Protocol too short!"));
        break;
      }

      uint8_t ptcSzArr[2], aux=0;
      wsClient.read(protocol, MAX_SZ_PROTOCOL);

      plc.set_ptcSz(consume_bytes<uint16_t>(ptcSzArr, aux));

      if(msgSz>MAX_SZ_PROTOCOL){
        Serial.println(F("ERROR: PLC Protocol too long!"));
        break;
      }

      plc.reset_protocol();
      uint8_t *protocol = plc.getProtocol();
      ptcSz = wsClient.read(protocol, MAX_SZ_PROTOCOL);
      plc.set_dev_vars_diag();

      break;
    default: break;
  }

  if(msgSz>MAX_SZ_PROTOCOL)
    
  else protocol.readWsProtocol(wsClient);
  
}

void Connect::websocket(){
  if(!websocket_connect()) return; //assegurar conexao

  websocket_msg_handler();
}

void Connect::protocol_run_diag(){
  protocol.run_diag();
}