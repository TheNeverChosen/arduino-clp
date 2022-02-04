#include <Arduino.h>
#include <stdint.h>
#include "led_monitor.h"
#include "connect.h"

enum Leds{LED_RUN_RED=1, LED_RUN_GREEN=2, LED_RUN_BLUE=3, LED_CON=4};

void led_monitor_reset(){
  pinMode(LED_RUN_RED,OUTPUT);
  pinMode(LED_RUN_GREEN,OUTPUT);
  pinMode(LED_RUN_BLUE,OUTPUT);
  pinMode(LED_CON,OUTPUT);

  led_monitor_set_wsConnection(true);
  led_monitor_set_run(PLC_RUN_ST_IDLE);
}

void led_monitor_set_wsConnection(bool status){
  digitalWrite(LED_CON,status ? HIGH:LOW);
}

void led_monitor_set_run(enum PlcRunStates status){
  switch(status){
    PLC_RUN_ST_IDLE:
      analogWrite(LED_RUN_RED,0);
      analogWrite(LED_RUN_GREEN,0);
      analogWrite(LED_RUN_BLUE,0);
      break;
    PLC_RUN_ST_SETTING:
      analogWrite(LED_RUN_RED,255);
      analogWrite(LED_RUN_GREEN,255);
      analogWrite(LED_RUN_BLUE,0);
      break;
    PLC_RUN_ST_PAUSED:
      analogWrite(LED_RUN_RED,255);
      analogWrite(LED_RUN_GREEN,0);
      analogWrite(LED_RUN_BLUE,0);
      break;
    PLC_RUN_ST_RUNNING:
      analogWrite(LED_RUN_RED,0);
      analogWrite(LED_RUN_GREEN,255);
      analogWrite(LED_RUN_BLUE,0);
      break;
    default: break;
  }
}

