#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);

char serverAddress[] = "192.168.100.10";
int port = 3333;

EthernetClient etClient;
WebSocketClient wsClient(etClient, serverAddress, port);

/*
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length){
    switch(type) {
        case WStype_DISCONNECTED:
          Serial.println("[WSc] Disconnected!\n");
          break;

        case WStype_CONNECTED:
          Serial.print("[WSc] Connected to url: ");
          Serial.println((char *) payload);
          break;
        
        case WStype_TEXT:
          Serial.print("[WSc] get text: ");
          Serial.println((char *)payload);
          break;
        
        case WStype_BIN:
          Serial.print("[WSc] get binary length: ");
          Serial.println(length);
          break;
      }
}*/

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
    int messageSize = wsClient.parseMessage();

    if (messageSize > 0) {
      if(wsClient.messageType()==TYPE_TEXT){
        Serial.println(F("Received a message:"));
        Serial.println(wsClient.readString());
      }
      else if(wsClient.messageType()==TYPE_BINARY){
        Serial.print(F("Tamanho do Binario: ")); Serial.println(messageSize);
        Serial.print(F("["));
        for(int i=0;i<messageSize;i++){
          if(i!=0) Serial.print(", ");
          Serial.print(wsClient.read()); 
        }
        Serial.println("]");
      }
    }

    // wait 5 seconds
    delay(1000);
  }

  Serial.println(F("disconnected"));
  
}
