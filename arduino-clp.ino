#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "connect.h"
#include "env.h"

Connect connect;

void setup(){
  Serial.begin(BAUD_RATE);
  while(!Serial){}

  connect.begin_network();
  setPinLedsMonitor();
}

void loop(){


  while(wsClient.connected()){
    int msgSz = wsClient.parseMessage();
    if (msgSz > 0){
      #ifdef DEBUG_ON
        Serial.print("MESSAGE RECEIVED. SIZE: "); Serial.println(msgSz);
      #endif
      if(wsClient.messageType()==TYPE_TEXT){
        Serial.println(F("Received a message:"));
        Serial.println(wsClient.readString());
      }
      else if(wsClient.messageType()==TYPE_BINARY){
        if(msgSz>MAX_SZ_PROTOCOL)
          Serial.println(F("ERROR: Message too long!"));
        else protocol.readWsProtocol(wsClient);
      }
      while(wsClient.available()) wsClient.read(); //consuming all left
    }

  }
  
  if(disconnecting==0){
    Serial.println(F("Disconnected. Trying to reconnect..."));
    disconnecting=1;
  }


  connect.websocket();
  connect.protocol_run_diag();
}