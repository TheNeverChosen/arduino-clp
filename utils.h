#pragma once

#include <Arduino.h>
#include <stdlib.h>
#include <stdint.h>
#include "env.h"

#define npos ((size_t)-1)

//Bin Search
template<typename T>
size_t smallest_greater(T *arr, size_t n, T k){
  if(arr==nullptr || n==0 || n==npos) return 0;

  size_t l=0, r = n-1, found=n, mid;

  #ifdef DEBUG_ON
    Serial.print("N: "); Serial.println(n);
    Serial.print("K: "); Serial.println(k);
  #endif

  while(l<=r && r!=npos){
    mid=(l+r)/2;
    #ifdef DEBUG ON
     Serial.print("L: "); Serial.println(l);
     Serial.print("R: "); Serial.println(r);
     Serial.print("MID: "); Serial.println(mid);
    #endif
    if(arr[mid]>=k){
      found=mid;
      #ifdef DEBUG_ON
        Serial.print("New Found: "); Serial.println(found);
      #endif
      r=mid-1;
    }
    else l=mid+1;
  }

  #ifdef DEBUG_ON
    Serial.println("Found: "); Serial.println(found);
  #endif
  return found;
}

uint8_t get_bit_arr(uint8_t *arr, size_t i, size_t n);