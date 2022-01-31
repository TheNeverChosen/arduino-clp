#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#include "CLPIO.h"
#include "Ladder.h"
#include "Protocol.h"
#include "env.h"
#include "utils.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);
char serverAddress[] = "34.95.137.51";
int port = 80;

uint8_t disconnecting =0;
Protocol protocol;

EthernetClient etClient;
WebSocketClient wsClient(etClient, serverAddress, port);

void setup(){
  Serial.begin(BAUD_RATE);
  while(!Serial){}
  
  if(Ethernet.begin(mac) == 0){
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
  }

  // protocol.reset_protocol(protocololo, sizeof(protocololo));
  // protocol.set_dev_vars_diag();
  Serial.println(F("starting WebSocket client"));
}

void loop(){
  wsClient.begin("/arduino");

  if(wsClient.connected()){ //PLC Identification
    uint8_t plcVersion[3];
    getVersion(plcVersion);

    wsClient.beginMessage(TYPE_BINARY);
    wsClient.write(0);
    wsClient.write(plcVersion, 3);
    wsClient.write(getReference());
    wsClient.endMessage();
    disconnecting=0;
  }

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

    protocol.run_diag();
  }
  if(disconnecting==0){
    Serial.println(F("Disconnected. Trying to reconnect..."));
    disconnecting=1;
  }

  protocol.run_diag();
}