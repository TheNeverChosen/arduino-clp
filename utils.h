#pragma once

#include <Arduino.h>
#include <stdlib.h>
#include <stdint.h>
#include "env.h"

////////////////////////////comsume_bytes////////////////////////////
///////////////////pegar duas posições de um array///////////////////
template<typename T, typename I_TP>
T consume_bytes(uint8_t *arr, I_TP &i){
  T res = *((T*)(&arr[i]));
  i+=sizeof(T);
  return res;
}

template<typename T>
T consume_bytes(uint8_t *arr){
  T res = *((T*)(arr));
  return res;
}

//Bin Search
template<typename T>
size_t smallest_greater(T *arr, size_t n, T k){
  if(arr==nullptr || n==0 || n==npos) return 0;

  size_t l=0, r = n-1, found=n, mid;

  #ifdef DEBUG_ON
    Serial.print(F("N: ")); Serial.println(n);
    Serial.print(F("K: ")); Serial.println(k);
  #endif

  while(l<=r && r!=npos){
    mid=(l+r)/2;
    #ifdef DEBUG_ON
     Serial.print(F("L: ")); Serial.println(l);
     Serial.print(F("R: ")); Serial.println(r);
     Serial.print(F("MID: ")); Serial.println(mid);
    #endif
    if(arr[mid]>=k){
      found=mid;
      #ifdef DEBUG_ON
        Serial.print(F("New Found: ")); Serial.println(found);
      #endif
      r=mid-1;
    }
    else l=mid+1;
  }

  #ifdef DEBUG_ON
    Serial.println(F("Found: ")); Serial.println(found);
  #endif
  return found;
}

uint8_t get_bit_arr(uint8_t *arr, size_t i, size_t n);