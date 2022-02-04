#include <Arduino.h>
#include <avr/pgmspace.h>

#include "env.h"

void get_version(uint8_t * plcVersion){
  for(byte i=0;i<verSz;i++)
    plcVersion[i] = pgm_read_byte_near(version + i);
}
uint8_t get_reference(){
  return pgm_read_byte_near(&reference);
}