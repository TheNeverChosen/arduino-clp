#include <Arduino.h>
#include <stdint.h>
#include "led_monitor.h"

enum Leds{LED_RUN_RED=1, LED_RUN_GREEN=2, LED_RUN_BLUE=3, LED_CON=4};

void set_led_monitor(){
  pinMode(LED_RUN_RED,OUTPUT);
  pinMode(LED_RUN_GREEN,OUTPUT);
  pinMode(LED_RUN_BLUE,OUTPUT);
  pinMode(LED_CON, OUTPUT);

  set_led_wsConection(LOW);
  set_led_run(ST_LED_RUN_IDLE);
}

void set_led_wsConection(uint8_t status){
  digitalWrite(LED_CON,status ? HIGH:LOW);
}

void set_led_run(enum LedStatus status){
  switch(status){
    ST_LED_RUN_IDLE:
      analogWrite(LED_RUN_RED,0);
      analogWrite(LED_RUN_GREEN,0);
      analogWrite(LED_RUN_BLUE,0);
      break;
    ST_LED_RUN_RECEIVED:
      analogWrite(LED_RUN_RED,255);
      analogWrite(LED_RUN_GREEN,255);
      analogWrite(LED_RUN_BLUE,0);
      break;
    ST_LED_RUN_RUNNING:
      analogWrite(LED_RUN_RED,0);
      analogWrite(LED_RUN_GREEN,255);
      analogWrite(LED_RUN_BLUE,0);
      break;
    default: break;
  }
}

