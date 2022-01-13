#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>
#include "env.h"

#define MIN(x, y) (x<y ? x : y)

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);

char serverAddress[] = "192.168.100.10";
int port = 3333;

EthernetClient etClient;
WebSocketClient wsClient(etClient, serverAddress, port);

void setup(){
  Serial.begin(9600);
  while(!Serial){}
  
  if(Ethernet.begin(mac) == 0){
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
  }

}
int count=0;
int received=1;
void loop(){
  wsClient.begin("/arduino");
  Serial.println(F("starting WebSocket client"));

  while (wsClient.connected()) {
      Serial.print(F("Sending hello "));
      Serial.println(count);

      // send a hello #
      wsClient.beginMessage(TYPE_TEXT);
      wsClient.print(F("hello "));
      wsClient.print(count);
      wsClient.endMessage();

      // increment count for next message
      count++;


    // check if a message is available to be received
    int messageSize = wsClient.parseMessage(); //170

    int buffSz = 50;
    uint8_t buffer[buffSz];

    if (messageSize > 0) {
      if(wsClient.messageType()==TYPE_TEXT){
        Serial.println(F("Received a message:"));
        Serial.println(wsClient.readString());
      }
      else if(wsClient.messageType()==TYPE_BINARY){
        Serial.print(F("Tamanho do Binario: ")); Serial.println(messageSize);
        Serial.print(F("["));
        
        if(messageSize>MAX_SZ_PROTOCOL){
          Serial.println("ERRO: MENSAGEM MUITO GRANDE!!!");
        }

        uint8_t array[MAX_SZ_PROTOCOL];
        size_t arraySz=0;

        wsClient.read(array, messageSize);

        for(int toRead=min(messageSize, buffSz);wsClient.read(buffer, toRead)>0;toRead=min(messageSize, buffSz)){ 
          memcpy(array+arraySz, buffer, toRead * sizeof(uint8_t));

          messageSize-=toRead;
          arraySz+=toRead;
        }
      }
    }

    // wait 5 seconds
    delay(1000);
  }

  Serial.println(F("disconnected"));
  
}
