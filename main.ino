#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdint.h>

////////////////////////////////////////////ENTRADAS////////////////////////////////
////////////////////////////////////////pinos e variaveis analogicos////////////////
const int pinoPOT = A0;
float leituraPOT = 0;
const int pinoLDR = A1;
int leituraLDR = 0;
////////////////////////////////////////pinos e variaveis analogicos////////////////

/* Legenda:
  * -> { } : inicio/fim de linha
  * -> [ ] : inicio/fim de bifurcação
  * -> ( ) : inicio/fim de caminho da bifurcação
  * -> C c: contato normalmente aberto / normalmente fechado
  * -> B b: bobina normalmente aberta / normalmente fechada */
/*
6 --quantidade de variaveis (bobinas, sensores, etc - para alocar no arduino)
4 1 --sensores atuadores/reles

--(Descrição dos sensores)
1 D 2 --(variavelLadder tipo[Digital] pinArduino)
4 D 3
0 A 3 347 1 --(variavelLadder tipo[Analogica] pinArduino divisor inicio[true/false])
3 A 5 450 0

--(Descriçao dos atuadores/reles)
5 4 --(variavelLadder pinArduino)

{c0[(C1)(C2)]c5B2}
{c3[(C4)(C5)]c2B5}
*/

/*
Dispositivo
  Entradas(sensores)
    -Analogicos
    -Digitais
  Saidas
    -Digitais

Digitais
  -Entrada
  -Saida
Analogicos
  -Entrada
*/
////////////////////////////////////////pinos e variaveis digitais//////////////////
//const int pinoChaveOptica = 4;
//int statusChaveO = LOW;

const int pinoChaveFimCurso = 5;
int statusChaveF = LOW;
const int pinoChave = 6;
int statusChave = LOW;
const int DS18B20 = 3;
float temperatura = 0;

OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);
////////////////////////////////////////pinos e variaveis digitais//////////////////
////////////////////////////////////////////ENTRADAS////////////////////////////////


////////////////////////////////////////////SAIDAS//////////////////////////////////
const int rCanal1 = 8;
const int rCanal2 = 9;
////////////////////////////////////////////SAIDAS//////////////////////////////////


void setup() {
  pinMode(pinoPOT, INPUT); 
  pinMode(pinoChaveFimCurso, INPUT);
  pinMode(pinoChave, INPUT);
  //pinMode(pinoChaveOptica, INPUT);
  pinMode(pinoLDR,INPUT);
  sensors.begin();

  pinMode(rCanal1,OUTPUT);
  digitalWrite(rCanal1,HIGH);
  
  pinMode(rCanal2,OUTPUT);
  digitalWrite(rCanal2,HIGH);

  Serial.begin(9600);
}

void loop(){
  sensors.requestTemperatures();
  
  temperatura = sensors.getTempCByIndex(0);
  
  leituraPOT = map(analogRead(pinoPOT), 0, 500, 0, 1023);
  statusChaveF = digitalRead(pinoChaveFimCurso);
  statusChave = digitalRead(pinoChave);
  //statusChaveO = digitalRead(pinoChaveOptica);
  leituraLDR = analogRead(pinoLDR);
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura); 
  Serial.println("*C");
  
  Serial.print("chave fim de curso ");
  if(statusChaveF==HIGH){Serial.println("fechda");}
  else{Serial.println("aberta");}

  Serial.print("chave ");
  if(statusChave==HIGH){Serial.println("fechda");}
  else{Serial.println("aberta");}

  Serial.print("potenciometro: ");
  Serial.println(leituraPOT);
  
  Serial.print("LDR: ");
  Serial.println(leituraLDR);
  delay(500);
}
