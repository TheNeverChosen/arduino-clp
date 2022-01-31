#include <Arduino.h>

#include "env.h"

void getVersion(uint8_t * plcVersion){
    for(byte i=0;i<verSz;i++)
        plcVersion[i] = pgm_read_byte_near(version + i);
}
uint8_t getReference(){
    return pgm_read_byte_near(&reference);
}
