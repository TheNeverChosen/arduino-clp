#pragma once

#include <stdlib.h>
#include <stdint.h>

//Bin Search
template<typename T>
size_t smallest_greater(T *arr, size_t n, T k){
  if(arr==nullptr || n==0) return 0;
  size_t l=0, r=n-1, found=n, mid;

  while(l<=r){
    mid=(l+r)/2;
    if(arr[mid]>=k){
      found=mid;
      r=mid-1;
    }
    else l=mid+1;
  }

  return found;
}

uint8_t get_bit_arr(uint8_t *arr, size_t i, size_t n);