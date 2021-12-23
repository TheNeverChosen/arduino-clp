#include <stdint.h>
#include "utils.h"

//Bin Search
template<typename T>
int smallest_greater(T *arr, int n, T k){
  if(arr==nullptr || n==0) return 0;
  int l=0, r=n-1, found=n, mid;

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

template <typename SZ_T>
uint8_t get_bit_arr(uint8_t *arr, SZ_T i, SZ_T n){
  uint8_t block = arr[i/8], qtShifts=8-1-(i%8);
  block>>=qtShifts;
  return block&1;
}